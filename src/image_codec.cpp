#include "image_codec.h"
#include <cmath>
#include <numeric>
#include <algorithm>

ImageCodec::ImageCodec(Predictor pred) : predictor(pred), optimalM(8) {}

int ImageCodec::paethPredictor(int a, int b, int c) const {
    int p = a + b - c;
    int pa = std::abs(p - a);
    int pb = std::abs(p - b);
    int pc = std::abs(p - c);
    
    if (pa <= pb && pa <= pc) return a;
    if (pb <= pc) return b;
    return c;
}

int ImageCodec::predictPixel(const std::vector<unsigned char>& image, int width, int x, int y) const {
    // Get adjacent pixel values, use 0 for edges
    int left = (x > 0) ? image[y * width + x - 1] : 0;
    int above = (y > 0) ? image[(y - 1) * width + x] : 0;
    int upperLeft = (x > 0 && y > 0) ? image[(y - 1) * width + x - 1] : 0;
    
    switch (predictor) {
        case Predictor::PREV_PIXEL:
            return left;
        case Predictor::ABOVE_PIXEL:
            return above;
        case Predictor::AVERAGE_PREDICTOR:
            return (left + above) / 2;
        case Predictor::PAETH_PREDICTOR:
            return paethPredictor(left, above, upperLeft);
        case Predictor::JPEG_LS_PREDICTOR:
            return left + above - upperLeft;
        case Predictor::GRADIENT_PREDICTOR:
            return left + (above - upperLeft) / 2;
        default:
            return 0;
    }
}

unsigned int ImageCodec::calculateOptimalM(const std::vector<int>& residuals) const {
    // Calculate optimal M based on the mean of absolute residuals
    if (residuals.empty()) return 8;  // Default value
    
    double sum = 0.0;
    for (int residual : residuals) {
        sum += std::abs(residual);
    }
    double mean = sum / residuals.size();
    
    // M should be close to -1/log2(P(X=0))
    // For exponential distribution of residuals, this is approximately mean/0.69
    if (mean < 1e-10) return 1;

    double p = 1.0 / (mean + 1.0);
    double optimalM = -1.0 / std::log2(1.0 - p);

    return std::max(1U, static_cast<unsigned int>(std::round(optimalM)));
}

std::vector<bool> ImageCodec::encode(const std::vector<unsigned char>& image, int width, int height) {
    std::vector<int> residuals;
    residuals.reserve(width * height);
    
    // First pass: calculate residuals and optimal M
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int predicted = predictPixel(image, width, x, y);
            int actual = image[y * width + x];
            residuals.push_back(actual - predicted);
        }
    }
    
    // Calculate optimal M value for Golomb coding
    optimalM = calculateOptimalM(residuals);
    
    // Initialize Golomb coder with optimal M
    Golomb coder(optimalM);
    
    // Store width, height, and M value in the encoded stream (as 16-bit values)
    std::vector<bool> encoded;
    encoded.reserve(width * height * 8);  // Rough estimate of size needed
    
    // Store header information (width, height, M, predictor)
    for (int i = 15; i >= 0; i--) encoded.push_back((width >> i) & 1);
    for (int i = 15; i >= 0; i--) encoded.push_back((height >> i) & 1);
    for (int i = 15; i >= 0; i--) encoded.push_back((optimalM >> i) & 1);
    for (int i = 3; i >= 0; i--) encoded.push_back((static_cast<int>(predictor) >> i) & 1);
    for (int i = 0; i < 12; i++) encoded.push_back(false);
    
    // Encode all residuals
    for (int residual : residuals) {
        coder.encodeTo(residual, encoded);
    }
    
    return encoded;
}

std::vector<unsigned char> ImageCodec::decode(const std::vector<bool>& encoded, int width, int height) {
    std::vector<unsigned char> image(width * height);
    
    // Read M value from header (it's after width and height, 32 bits in)
    unsigned int storedM = 0;
    for (int i = 32; i < 48; i++) {
        storedM = (storedM << 1) | encoded[i];
    }
    
    // Create Golomb coder with the M value from the header
    Golomb coder(storedM);
    size_t bitPos = 64;  // Skip header (16+16+16+2 bits)
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Decode the residual
            auto result = coder.decode(encoded, bitPos);
            bitPos += result.bitsConsumed;
            
            // Predict pixel value
            int predicted = predictPixel(image, width, x, y);
            
            // Reconstruct pixel value
            int pixelValue = predicted + result.value;
            // Clamp to valid range [0, 255]
            pixelValue = std::max(0, std::min(255, pixelValue));
            
            image[y * width + x] = static_cast<unsigned char>(pixelValue);
        }
    }
    
    return image;
}
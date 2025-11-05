#include "golomb.h"
#include <iostream>
#include <iomanip>

using namespace std;

void testEncoding(Golomb& golomb, int value){
    cout << "Value: " << setw(4) << value << " -> ";
    
    vector<bool> encoded = golomb.encode(value);
    cout << "Encoded: " << Golomb::bitsToString(encoded);
    cout << " (" << encoded.size() << " bits) -> ";
    
    auto decoded = golomb.decode(encoded);
    cout << "Decoded: " << decoded.value;
    
    if(decoded.value == value){
        cout << " ✓" << endl;
    }else{
        cout << " ✗ ERROR!" << endl;
    }
}

void testMode(const string& modeName, Golomb::NegativeMode mode){
    cout << "\n" << string(60, '=') << endl;
    cout << "Testing " << modeName << " mode with m=5" << endl;
    cout << string(60, '=') << endl;
    
    Golomb golomb(5, mode);
    
    cout << "\nPositive values:" << endl;
    for(int i = 0; i <= 10; i++){
        testEncoding(golomb, i);
    }
    
    cout << "\nNegative values:" << endl;
    for(int i = -1; i >= -10; i--){
        testEncoding(golomb, i);
    }
}

void testDifferentM(){
    cout << "\n" << string(60, '=') << endl;
    cout << "Testing different m values (Interleaving mode)" << endl;
    cout << string(60, '=') << endl;
    
    vector<unsigned int> m_values = {2, 3, 4, 8, 16};
    for(unsigned int m : m_values) {
        cout << "\nm = " << m << ":" << endl;
        Golomb golomb(m, Golomb::NegativeMode::INTERLEAVING);
        for(int i = 0; i <= 5; i++){
            testEncoding(golomb, i);
        }
    }
}

void testAdaptiveM(){
    cout << "\n" << string(60, '=') << endl;
    cout << "Testing adaptive m (changing m during execution)" << endl;
    cout << string(60, '=') << endl;
    
    Golomb golomb(4, Golomb::NegativeMode::INTERLEAVING);
    
    cout << "\nWith m=4:" << endl;
    testEncoding(golomb, 10);
    testEncoding(golomb, 15);
    
    golomb.setM(8);
    cout << "\nWith m=8:" << endl;
    testEncoding(golomb, 10);
    testEncoding(golomb, 15);
    
    golomb.setM(2);
    cout << "\nWith m=2:" << endl;
    testEncoding(golomb, 10);
    testEncoding(golomb, 15);
}

int main(){
    cout << "GOLOMB CODING TEST PROGRAM" << endl;
    cout << string(60, '=') << endl;
    
    try{
        testMode("SIGN-MAGNITUDE", Golomb::NegativeMode::SIGN_MAGNITUDE);
        testMode("INTERLEAVING", Golomb::NegativeMode::INTERLEAVING);
        testDifferentM();
        testAdaptiveM();
        
        cout << "\n" << string(60, '=') << endl;
        cout << "All tests completed successfully!" << endl;
        cout << string(60, '=') << endl;
        
    }catch(const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include "GlassesDisplay.hpp"

using std::string;
using std::ifstream;
using std::cout;
using std::endl;

// function to return the hash value based on the first digit
unsigned int hashfct1(unsigned int barcode) {
  // TO BE COMPLETED
    unsigned int d=0;
    d = (barcode / 1000000 % 10);
    return d;
}

// function to return the hash value based on the second digit
unsigned int hashfct2(unsigned int barcode) {
  // TO BE COMPLETED
    unsigned int d=0;
    d = (barcode / 100000 % 10);
    return d;
}

// function to return the hash value based on the third digit
unsigned int hashfct3(unsigned int barcode) {
  // TO BE COMPLETED
    unsigned int d=0;
    d = (barcode / 10000 % 10);
    return d;
}

// function to return the hash value based on the fourth digit
unsigned int hashfct4(unsigned int barcode) {
  // TO BE COMPLETED
    unsigned int d=0;
    d = (barcode / 1000 % 10);
    return d;
}

// function to return the hash value based on the fifth digit
unsigned int hashfct5(unsigned int barcode) {
    // TO BE COMPLETED
    unsigned int d=0;
     d = (barcode / 100 % 10);
    return d;
}

// function to return the hash value based on the sixth digit
unsigned int hashfct6(unsigned int barcode) {
    // TO BE COMPLETED
    unsigned int d=0;
     d = (barcode / 10 % 10);
    return d;
}

// function to return the hash value based on the seventh digit
unsigned int hashfct7(unsigned int barcode) {
    // TO BE COMPLETED
    unsigned int d=0;
     d = (barcode / 1 % 10);
    return d;
}


// Constructor for struct Item
Glasses::Glasses(string glassesColor, string glassesShape, string glassesBrand, unsigned int barcode): glassesColor_(glassesColor), glassesShape_(glassesShape), glassesBrand_(glassesBrand), barcode_(barcode)
{};

// Load information from a text file with the given filename
// THIS FUNCTION IS COMPLETE
void GlassesDisplay::readTextfile(string filename) {
  ifstream myfile(filename);

  if (myfile.is_open()) {
    cout << "Successfully opened file " << filename << endl;
    string glassesColor;
    string glassesShape;
    string glassesBrand;
    unsigned int barcode;
    while (myfile >> glassesColor >> glassesShape >> glassesBrand >> barcode) {
			if (glassesColor.size() > 0)
      	addGlasses(glassesColor, glassesShape, glassesBrand, barcode);
    }
    myfile.close();
  }
  else
    throw std::invalid_argument("Could not open file " + filename);
}

void GlassesDisplay::addGlasses(string glassesColor, string glassesShape, string glassesBrand, unsigned int barcode) {
  // TO BE COMPLETED
  // function that adds the specified pair of glasses to main display (i.e., to all hash tables)

    // create a Glasses object
    Glasses new_glasses(glassesColor, glassesShape, glassesBrand, barcode);

    // insert the object into each of the seven hashtables
    hT1.insert({barcode, new_glasses});
    hT2.insert({barcode, new_glasses});
    hT3.insert({barcode, new_glasses});
    hT4.insert({barcode, new_glasses});
    hT5.insert({barcode, new_glasses});
    hT6.insert({barcode, new_glasses});
    hT7.insert({barcode, new_glasses});

    // ensure all hash tables have the same size after insertion
    if (this->size() != hT1.size()) 
    {
        throw std::length_error("Hash table sizes are not the same after insertion");
    }

}

bool GlassesDisplay::removeGlasses(unsigned int barcode) {
  // TO BE COMPLETED
  // function that removes the pair of glasses specified by the barcode from the display
  // if pair is found, then it is removed and the function returns true
  // else returns false

  // remove the pair of glasses given the barcode from all 7 hashtables, return true if successful, otherwise return false
  if (hT1.erase(barcode) && hT2.erase(barcode) && hT3.erase(barcode) && hT4.erase(barcode) && hT5.erase(barcode) && hT6.erase(barcode) && hT7.erase(barcode)) 
  {
      // ensure all hash tables have the same size after removal
      if (this->size() != hT1.size()) 
      {
          throw std::length_error("Hash table sizes are not the same after removal");
      }
      return true;
  } 
  else 
  {
      return false;   //if not found return false
  }
}

unsigned int GlassesDisplay::bestHashing() {
  // TO BE COMPLETED
  // function that decides the best has function, i.e. the ones among
  // fct1-fct7 that creates the most balanced hash table for the current
  // GlassDisplay data member allGlasses

	// Hints:
	// Calculate the balance of each hashtable, one by one.
	/*
	 for (unsigned i=0; i<10; ++i) {
    cout << hT1.bucket_size(i); // Call bucket_size() to get the size of each bucket
  }
	*/
	// Then, calculate the lowest balance

  // Find the most balanced hash function

    //create an array of pointers to the 7 hashtables
    CustomHashTable* hTs[7] = {&hT1, &hT2, &hT3, &hT4, &hT5, &hT6, &hT7};
    unsigned int min_balance = -1;
    unsigned int best_hash = 0;
    //for loop to loop over each hashtable
    for (unsigned int i = 0; i < 7; ++i) 
    {
        unsigned int min_bucket_size = -1;
        unsigned int max_bucket_size = 0;
        //for loop to get the size of each bucket from current hashtable
        for (unsigned int j = 0; j < 10; ++j) 
        {
            unsigned int current_bucket_size = hTs[i]->bucket_size(j);  //get bucket size of each bucket
            //if bucket size is less than min bucket size, update value
            if (current_bucket_size < min_bucket_size)
            {
              min_bucket_size = current_bucket_size;
            }
            //if bucket size is greater than max bucket size, update value
            if (current_bucket_size > max_bucket_size)
            {
              max_bucket_size = current_bucket_size;
            }
        }
        //calculate the balance for current hashtable
        unsigned int current_balance = max_bucket_size - min_bucket_size;   
        //compare and update the best hash variable if necessary
        if (current_balance < min_balance) 
        {
            min_balance = current_balance;
            best_hash = i + 1;
        }
    }
    return best_hash;
}

// ALREADY COMPLETED
size_t GlassesDisplay::size() {
    if ((hT1.size() != hT2.size()) || (hT1.size() != hT3.size()) || (hT1.size() != hT4.size()) || (hT1.size() != hT5.size())|| (hT1.size() != hT6.size()) || (hT1.size() != hT7.size()))
  	throw std::length_error("Hash table sizes are not the same");
    
	return hT1.size();
}


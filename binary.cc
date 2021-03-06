// Copyright 2015-2018 RWTH Aachen University
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "binary.hh"

#include <stdexcept>

using namespace std;

//adds the bits to the end of the bits set
void Binary::push_back(uint64_t bits, uint8_t numberOfBits)  {
    //numberOfBits must be smaller than the max number of bits in the return type
    assert(numberOfBits <= 64 && numberOfBits > 0);

    //add the bits to the end
    uint32_t oldBitOffset = bitOffset;
    bitOffset = nextFreeBit; 

    if(mem == NULL)  { //init the memory, if needed
        uint8_t neededBytes = ceil(numberOfBits / 8.); //space in bytes that we need
        uint16_t allocate = initMallocSize; //initMallocSize can be used to preserve space in advance (min. reallocs)
        if(neededBytes > initMallocSize) //ensure that we get enough space to avoid reallocs
            allocate = neededBytes;
        mem = (uint8_t *) malloc(allocate);
        if(mem != NULL)  {
            allocatedBytes = allocate;
        }
        else
            throw "Malloc failed!"; 
    }
    else  {
        uint16_t neededBytes = ceil((bitOffset + numberOfBits) / 8.);
        if(neededBytes > allocatedBytes)  { //just reserve new space if needed
            //double the allocated memory in this case to reduce the number of needed reallocs 
            uint32_t newAllocatedBytes = allocatedBytes * 2;
            uint8_t *new_mem = (uint8_t *) realloc(mem, newAllocatedBytes);
            if(new_mem != NULL)  {
                mem = new_mem;
                allocatedBytes = newAllocatedBytes;
            }
            else
                throw "Reallocation failed!"; 
        }
    }
    
    //add the bits to the binary memory
    //MSB of value should be the MSB of the value in the binary
    for(int16_t position = numberOfBits - 1; position >= 0; position--, bitOffset++)  {
        //get the bit value of position in the first bit
        uint8_t bit = (bits & (((uint64_t) 1) << position)) >> position; 
        uint8_t mask = 1 << (7 - bitOffset % 8);
        if(bit == 1) //set bit
            mem[bitOffset / 8] |= mask; //add the bit to the next free position in the binary
        else //clear bit
            mem[bitOffset / 8] &= ~mask; //add the bit to the next free position in the binary
    }

    nextFreeBit = bitOffset;  //store the position of the next free bit in the bit set
    bitOffset = oldBitOffset; //restore to the old position in the bit set
}

uint64_t Binary::next(uint8_t numberOfBits)  {
    //numberOfBits must be smaller than the max number of bits in the return type
    assert(numberOfBits <= 64 && numberOfBits > 0);
    assert(mem != NULL);

    uint64_t value = 0;
    for(int8_t valuePos = numberOfBits - 1; valuePos >= 0; valuePos--, bitOffset++)  { //mem is always from MSB to LSB
        //get the next bit from the memory
        uint64_t bit = (mem[bitOffset / 8] >> (7 - (bitOffset % 8))) & 1; //next mem bit at LSB
        bit <<= valuePos; //shift the bit from the memory to the right position
        value |= bit; //set the bit inside the value
    }

    return value;
}

uint64_t Binary::at(uint64_t offset, uint8_t numberOfBits)  {
    //numberOfBits must be smaller than the max number of bits in the return type
    assert(numberOfBits <= 64 && numberOfBits > 0);
    assert(mem != NULL);

    uint64_t value = 0;
    for(int8_t valuePos = numberOfBits - 1; valuePos >= 0; valuePos--, offset++)  { //mem is always from MSB to LSB
        //get the next bit from the memory
        uint64_t bit = (mem[offset / 8] >> (7 - (offset % 8))) & 1; //next mem bit at LSB
        bit <<= valuePos; //shift the bit from the memory to the right position
        value |= bit; //set the bit inside the value
    }

    return value;
}

uint64_t Binary::getPosition()  {
    return bitOffset;
}

void Binary::setPosition(uint64_t newPosition)  {
    bitOffset = newPosition;
}

void Binary::print()  {
    for(uint32_t position = 0; position < nextFreeBit; position++)    
        cout << at(position, 1);
    cout << endl;
}

void Binary::write(string filename)  {
    ofstream file(filename);
    if(!file.is_open())
        throw runtime_error("Can't open file "+ filename);
    // write output to filename
    for(uint32_t position = 0; position < nextFreeBit; position += 8)
        file << (uint8_t)( at(position, 8) >> (sizeof(uint64_t) - 8) );
    file.close();
}

void Binary::read_from_file(string filename)  {
    ifstream file(filename, ios::binary);
    if(!file.is_open())
        throw runtime_error("Can't open file "+ filename);
    //read the file into string
    uint8_t value;
    while(file.read((char *)(&value), sizeof(value))) {
        push_back(value, sizeof(value)*8);
    }
    file.close();
}

uint32_t Binary::size()  {
    return nextFreeBit;
}

uint8_t *Binary::getPointer()  {
    return mem;
}

#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;
#define ADDU 1
#define SUBU 3
#define AND 4
#define OR  5
#define NOR 7
#define MemSize 65536 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.


class RF
{
public:
    bitset<32> ReadData1, ReadData2;
    RF()
    {
        Registers.resize(32);
        Registers[0] = bitset<32> (0);
    }
    
    void ReadWrite(bitset<5> RdReg1, bitset<5> RdReg2, bitset<5> WrtReg, bitset<32> WrtData, bitset<1> WrtEnable)
    {
        ReadData1 = Registers[RdReg1.to_ulong()];
        ReadData2 = Registers[RdReg2.to_ulong()];
        if (WrtEnable.to_ulong() == 1)
            Registers[WrtReg.to_ulong()] = WrtData;
        
        // implement the funciton by you.
    }
    
    void OutputRF()
    {
        ofstream rfout;
        rfout.open("RFresult.txt",std::ios_base::app);
        if (rfout.is_open())
        {
            rfout<<"A state of RF:"<<endl;
            for (int j = 0; j<32; j++)
            {
                rfout << Registers[j]<<endl;
            }
            
        }
        else cout<<"Unable to open file";
        rfout.close();
        
    }
private:
    vector<bitset<32> >Registers;
    
};

class ALU
{
public:
    bitset<32> ALUresult;
    bitset<32> ALUOperation (bitset<3> ALUOP, bitset<32> oprand1, bitset<32> oprand2)
    {
        unsigned long a, b;
        a = oprand1.to_ulong();
        b = oprand2.to_ulong();
        if (ALUOP == 1)
            ALUresult = a+b;
        if (ALUOP == 3)
            ALUresult = a-b;
        if (ALUOP == 4)
            ALUresult = oprand1 & oprand2;
        if (ALUOP == 5)
            ALUresult = oprand1 | oprand2;
        if (ALUOP == 7)
            ALUresult = oprand1 ^ oprand2;
        // implement the ALU operations by you.
        return ALUresult;
    }
};

class INSMem
{
public:
    bitset<32> Instruction;
    INSMem()
    {       IMem.resize(MemSize);
        ifstream imem;
        string line;
        int i=0;
        imem.open("imem.txt");
        if (imem.is_open())
        {
            while (getline(imem,line))
            {
                IMem[i] = bitset<8>(line);
                i++;
            }
            
        }
        else cout<<"Unable to open file";
        imem.close();
        
    }
    
    bitset<32> ReadMemory (bitset<32> ReadAddress)
    {
        for (int i = 0; i < 8; i++) {
            Instruction[i+24] = IMem[ReadAddress.to_ulong()][i];
        }
        for (int i = 0; i < 8; i++) {
            Instruction[i+16] = IMem[ReadAddress.to_ulong()+1][i];
        }
        for (int i = 0; i < 8; i++) {
            Instruction[i+8] = IMem[ReadAddress.to_ulong()+2][i];
        }
        for (int i = 0; i < 8; i++) {
            Instruction[i] = IMem[ReadAddress.to_ulong()+3][i];
        }
        // implement by you. (Read the byte at the ReadAddress and the following three byte).
        return Instruction;
    }
    
private:
    vector<bitset<8> > IMem;
    
};

class DataMem
{
public:
    bitset<32> readdata;
    DataMem()
    {
        DMem.resize(MemSize);
        ifstream dmem;
        string line;
        int i=0;
        dmem.open("dmem.txt");
        if (dmem.is_open())
        {
            while (getline(dmem,line))
            {
                DMem[i] = bitset<8>(line);
                i++;
            }
        }
        else cout<<"Unable to open file";
        dmem.close();
        
    }
    bitset<32> MemoryAccess (bitset<32> Address, bitset<32> WriteData, bitset<1> readmem, bitset<1> writemem)
    {
        if (writemem == 1) {
            for (int i = 0; i < 8; i++)
                DMem[Address.to_ulong()][i] = WriteData[i+24];
            for (int i = 0; i < 8; i++)
                DMem[Address.to_ulong()+1][i] = WriteData[i+16];
            for (int i = 0; i < 8; i++)
                DMem[Address.to_ulong()+2][i] = WriteData[i+8];
            for (int i = 0; i < 8; i++)
                DMem[Address.to_ulong()+3][i] = WriteData[i];
        }
        if (readmem == 1) {
            for (int i = 0; i < 8; i++)
                readdata[i+24] = DMem[Address.to_ulong()][i];
            for (int i = 0; i < 8; i++)
                readdata[i+16] = DMem[Address.to_ulong()+1][i];
            for (int i = 0; i < 8; i++)
                readdata[i+8] = DMem[Address.to_ulong()+2][i];
            for (int i = 0; i < 8; i++)
                readdata[i] = DMem[Address.to_ulong()+3][i];
        }
        
        // implement by you.
        return readdata;
    }
    
    void OutputDataMem()
    {
        ofstream dmemout;
        dmemout.open("dmemresult.txt");
        if (dmemout.is_open())
        {
            for (int j = 0; j< 1000; j++)
            {
                dmemout << DMem[j]<<endl;
            }
            
        }
        else cout<<"Unable to open file";
        dmemout.close();
        
    }
    
private:
    vector<bitset<8> > DMem;
    
};



int main()
{
    RF myRF;
    ALU myALU;
    INSMem myInsMem;
    DataMem myDataMem;
    
    bitset<32> PC = 0;
    bitset<6>  opcode;
    bitset<3> ALUop;
    bitset<5> Rs;
    bitset<5> Rt;
    bitset<5> Rd;
    bitset<32> SignExImm;
    
    while (1)
    {
        // Fetch
        myInsMem.Instruction = myInsMem.ReadMemory(PC);
        
        // If current insturciton is "11111111111111111111111111111111", then break;
        if (myInsMem.Instruction == 0xffffffff) break;
        
        // decode(Read RF)
        for (int i = 0; i < 6; i++) {
            opcode[i] = myInsMem.Instruction[i+26];
        }
        if (opcode != 0)
            ALUop = 1;
        else {
            for (int i = 0; i < 3; i++) {
                ALUop[i] = myInsMem.Instruction[i];
            }
        }
        if (opcode.to_ulong() == 0x2b) {
            for (int i = 0; i < 5; i++) {
                Rs[i] = myInsMem.Instruction[21+i];
                Rt[i] = myInsMem.Instruction[16+i];
            }
            for (int i = 0; i < 16; i++)
                SignExImm[i] = myInsMem.Instruction[i];
            for (int i = 0; i < 16; i++)
                SignExImm[i+16] = 0;
            myRF.ReadWrite(Rs, Rt, 0, 0, 0);
            myALU.ALUOperation(ALUop, myRF.ReadData1, SignExImm);
            myDataMem.MemoryAccess(myALU.ALUresult, myRF.ReadData2, 0, 1);
            // sw
        }
        if (opcode.to_ulong() == 0x23) {
            for (int i = 0; i < 5; i++) {
                Rs[i] = myInsMem.Instruction[21+i];
                Rt[i] = myInsMem.Instruction[16+i];
            }
            for (int i = 0; i < 16; i++)
                SignExImm[i] = myInsMem.Instruction[i];
            for (int i = 0; i < 16; i++)
                SignExImm[i+16] = 0;
            myRF.ReadWrite(Rs, 0, Rt, 0, 0);
            myALU.ALUOperation(ALUop, myRF.ReadData1, SignExImm);
            myDataMem.MemoryAccess(myALU.ALUresult, 0, 1, 0);
            myRF.ReadWrite(Rs, 0, Rt, myDataMem.readdata, 1);
            // lw
        }
        if (opcode.to_ulong() == 4) {
            for (int i = 0; i < 5; i++) {
                Rs[i] = myInsMem.Instruction[21+i];
                Rt[i] = myInsMem.Instruction[16+i];
            }
            myRF.ReadWrite(Rs, Rt, 0, 0, 0);
            if (Rs == Rt) {
                for (int i = 0; i < 16; i++)
                    SignExImm[i+2] = myInsMem.Instruction[i];
                for (int i = 0; i < 14; i++)
                    SignExImm[i+18] = 0;
                SignExImm[0] = 0;
                SignExImm[1] = 0;
                PC = PC.to_ulong() + 4 + SignExImm.to_ulong();
            } else {
                PC = PC.to_ulong() + 4;
            }
            // beq
        }
        if (opcode.to_ulong() == 9) {
            for (int i = 0; i < 5; i++) {
                Rs[i] = myInsMem.Instruction[21+i];
                Rt[i] = myInsMem.Instruction[16+i];
            }
            for (int i = 0; i < 16; i++)
                SignExImm[i] = myInsMem.Instruction[i];
            for (int i = 0; i < 16; i++)
                SignExImm[i+16] = 0;
            myRF.ReadWrite(Rs, 0, Rt, 0, 0);
            myALU.ALUOperation(ALUop, myRF.ReadData1, SignExImm);
            myRF.ReadWrite(Rs, 0, Rt, myALU.ALUresult, 1);
            // addiu
        }
        if (opcode.to_ulong() == 2) {
            for (int i = 0; i < 4; i++)
                SignExImm[i+28] = myInsMem.Instruction[i+28];
            for (int i = 0; i < 26; i++)
                SignExImm[i+2] = myInsMem.Instruction[i];
            SignExImm[0] = 0;
            SignExImm[1] = 0;
            PC = SignExImm;
            // jump
        }
        if (opcode.to_ulong() == 0) {
            for (int i = 0; i < 5; i++) {
                Rs[i] = myInsMem.Instruction[21+i];
                Rt[i] = myInsMem.Instruction[16+i];
                Rd[i] = myInsMem.Instruction[11+i];
            }
            myRF.ReadWrite(Rs, Rt, Rd, 0, 0);
            myALU.ALUOperation(ALUop, myRF.ReadData1, myRF.ReadData2);
            myRF.ReadWrite(Rs, Rt, Rd, myALU.ALUresult, 1);
            // R-type
        }
        
        PC = PC.to_ulong() + 4;
        
        
        // Execute
        
        // Read/Write Mem
        
        // Write back to RF
        
        
        myRF.OutputRF(); // dump RF;
    }
    myDataMem.OutputDataMem(); // dump data mem
    
    return 0;
    
}
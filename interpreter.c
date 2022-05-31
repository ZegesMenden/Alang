#ifdef _WIN32
    #include <direct.h>
    #define getcwd _getcwd // stupid MSFT "deprecation" warning
#elif
    #include <unistd.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// ======================================================================

#define false 0
#define true 1

#define maxLineLen 100

// ======================================================================

// our 8 registers
int regs[8] = {0};

// our memory for the program
int progMem[1000];
size_t memsize = 1000;

typedef enum
{
    _err = -1,
    _ldr = 0,
    _sto = 1,
    _add = 2,
    _sub = 3,
    _mul = 4,
    _div = 5,
    _inV = 6,
    _out = 7,
    _jnz = 8
} operation_t;

typedef enum
{
    mem = 0,
    reg = 1,
    val = 2
} aType_t;

typedef struct 
{
    aType_t arg_type;
    int val;
} arg_t;

typedef struct 
{
    size_t position;
    operation_t op;
    arg_t lhs;
    arg_t rhs;
} instruction_t;

// ======================================================================

int _atoi(char *str)
{
    int tmp;
    for (int i = 0; str[i] != '\0'; ++i)
    {
        if ( str[i] >= 48 && str[i] <= 57 )
        {
            tmp = tmp * 10 + str[i] - '0';
        }
    }
    return tmp;
};

unsigned int hash(const char* data, size_t const size) {
    unsigned int _hash = 0;
    for ( int i = 0; i < size; i++ )
    {
        _hash += ((unsigned int)data[i])<<(i*8);
    }
    return _hash;
};

// ======================================================================
// linting and parsing
// ======================================================================

operation_t parseOp(char *instruction)
{
    // printf("size: %d\ndata: |%s|\n", strlen(instruction), instruction);
    if ( strlen(instruction) != 3 ) { return _err; }

    // printf("%d\n", hash("err", 3));
    // printf("%d\n", hash("ldr", 3));
    // printf("%d\n", hash("sto", 3));
    // printf("%d\n", hash("add", 3));
    // printf("%d\n", hash("sub", 3));
    // printf("%d\n", hash("mul", 3));
    // printf("%d\n", hash("div", 3));
    // printf("%d\n", hash("inV", 3));
    // printf("%d\n", hash("out", 3));
    // printf("%d\n", hash("jnz", 3));

    switch ( hash(instruction, 3) )
    {
        case(7500389) : { return _err; break; }
        case(7496812) : { return _ldr; break; }
        case(7304307) : { return _sto; break; }
        case(6579297) : { return _add; break; }
        case(6452595) : { return _sub; break; }
        case(7107949) : { return _mul; break; }
        case(7760228) : { return _div; break; }
        case(5664361) : { return _inV; break; }
        case(7632239) : { return _out; break; }
        case(8023658) : { return _jnz; break; }
        default : { return _err; break; }   
    }
}

arg_t parseArg(char *arg, unsigned int len)
{
    arg_t tmp;
    char *_tmp = arg;
    switch(arg[0])
    {
        case('%'):
        {
            _tmp++;
            tmp.arg_type = reg;
            break;
        };
        case('#'):
        {
            _tmp++;
            tmp.arg_type = mem;
            break;
        };
        default:
        {
            tmp.arg_type = val;
            break;
        }
    }
    tmp.val = atoi(_tmp);
    return tmp;
}

instruction_t parseInstruction(char *operation, char *lhs, char *rhs)
{
    instruction_t tmp;
    tmp.op = parseOp(operation);
    tmp.lhs = parseArg(lhs, sizeof(lhs));
    tmp.rhs = parseArg(rhs, sizeof(rhs));
    return tmp;
}

instruction_t lintLine(char *line)
{
    int index = 0;
    char *token = strtok(line, " ");

    instruction_t tmp;

    while ( token != NULL )
    {
        index ++;
        switch(index)
        {
            case(1):
            {
                tmp.op = parseOp(token);
                break;
            }
            case(2):
            {
                tmp.lhs = parseArg(token, sizeof(token));
                break;
            }
            case(3):
            {
                tmp.rhs = parseArg(token, sizeof(token));
                break;
            }
            default:
            {
                break;
            }
        }
        token = strtok(NULL, " ");
    }

    return tmp;

}

// ======================================================================

void infoFromOp(instruction_t op, char **op_t, char **lhs_t, char **rhs_t)
{

    switch(op.op)
    {
        default: {
            *op_t = "err";
            break;
        }
        case(_ldr): {
            *op_t = "ldr";
            break;
        }
        case(_sto): {
            *op_t = "sto";
            break;
        }
        case(_add): {
            *op_t = "add";
            break;
        }
        case(_sub): {
            *op_t = "sub";
            break;
        }
        case(_mul): {
            *op_t = "mul";
            break;
        }
        case(_div): {
            *op_t = "div";
            break;
        }
        case(_inV): {
            *op_t = "inV";
            break;
        }
        case(_out): {
            *op_t = "out";
            break;
        }
        case(_jnz): {
            *op_t = "jnz";
            break;
        }
    }

    switch(op.lhs.arg_type)
    {
        case(mem): {
            *lhs_t = "mem";
        }
        case(reg): {
            *lhs_t = "reg";
        }
        case(val): {
            *lhs_t = "val";
        }
    }

    switch(op.rhs.arg_type)
    {
        case(mem): {
            *rhs_t = "mem";
        }
        case(reg): {
            *rhs_t = "reg";
        }
        case(val): {
            *rhs_t = "val";
        }
    }

};

int getVal(arg_t op)
{
    switch(op.arg_type)
    {
        case(mem):
        {
            return progMem[op.val]; 
            break;
        }
        case(reg):
        {
            return regs[op.val];
            break;
        }
        case(val):
        {
            return op.val;
            break;
        }
    }
}

void setVal(arg_t op, int _val)
{
    switch(op.arg_type)
    {
        case(mem):
        {
            progMem[op.val] = _val; 
            break;
        }
        case(reg):
        {
            regs[op.val] = _val;
            break;
        }
        case(val):
        {
            break;
        }
    }
}

int main(int argc, char** argv)
{
    // prints funny verbose messages if true
    int verbose = 0;
    char *output = "";

    // if no file was provided
    if ( argc < 1 ) { printf("error! no file provided"); return 0; }

    // parse the arguments
    for ( int i = 0; i < argc; i++ )
    {
        if ( argv[i][0] == '-' )
        {
            if ( argv[i][1] == 'o' )
            {
                output = (char*)malloc(sizeof(argv[i+1]));
                output = argv[i+1];
            }
            else if ( argv[i][1] == 'v' )
            {
                verbose = true;
            }
        }
    }

    if ( strlen(output) != 0 )
    {
        printf("output: %s\n", output);
    }
    else
    {
        if (verbose)
            printf("no output!\n");
        char *tmpOut = (char*) malloc(strlen(getcwd(NULL, 0)) + strlen(argv[1]) + 1);
        // tmpOut[0] = '\0';
        strcpy(tmpOut, getcwd(NULL, 1));
        strcat(tmpOut, argv[1]);
        tmpOut[strlen(tmpOut)-3] = 'i';
        tmpOut[strlen(tmpOut)-2] = '\0';
        output = (char*) malloc(strlen(tmpOut)+1);
        strcpy(output, tmpOut);
        free(tmpOut);
        if (verbose)
            printf("output set to: %s\n", output);
    }

    FILE *fPtr;
    char path[256] = {0};
    strcpy(path, argv[1]);
    fPtr = fopen(path, "r");
    
    // somehow not needed?
    // if ( fPtr == NULL )
    // {
    //     printf("uh oh couldnt open the file!");
    //     memcpy(path, 0, 256);
    //     printf("cleared string: %s\n", path);
    //     strcat(path, getcwd(NULL, 0));
    //     strcat(path, argv[1]);
    //     printf("new string: %s\n", path);
    // }

    if ( fPtr == NULL ) { exit(1); }

    if (verbose)
        printf("opened %s\n", argv[1]);

    // current line
    unsigned long long lineN = 0;
    
    // string buffer from file
    char fBuf[maxLineLen] = {0};

    if (verbose)
        printf("reading\n");

    FILE *outPtr;
    outPtr = fopen(output, "wb");
    if ( !outPtr ) { printf("error opening %s!\n", output); return 1; }

    while(fgets(fBuf, maxLineLen, fPtr))
    {
        instruction_t tmp = lintLine(fBuf);
        tmp.position = lineN;
        if (tmp.op != _err)
        {
            fwrite(&tmp, sizeof(instruction_t), 1, outPtr);

            if (verbose)
            {
                char *rhst = "err";
                char *lhst = "err";
                char *opt  = "err";
                infoFromOp(tmp, &opt, &lhst, &rhst); 
                printf("\nline[%06d]: op: %s\nlhs type: %s, lhs value: %d\nrhs type: %s, rhs value: %d\n", lineN, opt, lhst, tmp.lhs.val, rhst, tmp.rhs.val);
            }
            lineN++;
        }
    }

    fclose(outPtr);
    fclose(fPtr);

    // ======================================================================
    // and now interpreting! 
    if (verbose)
        printf("running...\n");

    outPtr = fopen(output, "rb");
    unsigned long long instruction = 0;

    while (!feof(outPtr))
    {
        instruction_t iCurrent;
        fread(&iCurrent, sizeof(instruction_t), 1, outPtr);

        switch(iCurrent.op)
        {
            case(_err):
            {
                break;
            }
            case(_ldr):
            {
                regs[iCurrent.lhs.val] = getVal(iCurrent.rhs);
                break;
            }
            case(_sto):
            {
                setVal(iCurrent.lhs, getVal(iCurrent.rhs));
                break;
            }
            case(_add):
            {
                setVal(iCurrent.lhs, getVal(iCurrent.lhs) + getVal(iCurrent.rhs));
                break;
            }
            case(_sub):
            {
                setVal(iCurrent.lhs, getVal(iCurrent.lhs) - getVal(iCurrent.rhs));
                break;
            }
            case(_mul):
            {
                setVal(iCurrent.lhs, getVal(iCurrent.lhs) * getVal(iCurrent.rhs));
                break;
            }
            case(_div):
            {
                setVal(iCurrent.lhs, getVal(iCurrent.lhs) / getVal(iCurrent.rhs));
                break;
            }
            case(_inV):
            {
                int tmp;
                scanf("%d", &tmp);
                setVal(iCurrent.lhs, tmp);
                break;
            }
            case(_out):
            {
                printf("%d\n", getVal(iCurrent.lhs));
                break;
            }
            case(_jnz):
            {
                if ( getVal(iCurrent.lhs) != 0 ) { 
                    instruction_t _tmpInst;
                    rewind(outPtr);
                    for ( int i = 0; i < getVal(iCurrent.rhs); i++ )
                    {
                        fread(&_tmpInst, sizeof(instruction_t), 1, outPtr);
                    }
                }
                break;
            }
            if (verbose)
                printf("%d\n", iCurrent.position);
        }
    }    

    return 0;
}
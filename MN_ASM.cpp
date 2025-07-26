//               In the name of God
// MINI_ASM Compiler - Fixed Version
//   the lanuage not yet fully built.

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <cstdlib>

using namespace std;

// Helper macros
#define print(x) cout << (x) << endl
#define printn(x) cout << (x)
#define input(x) getline(cin, x)

// Variable types
enum class VarType {
    INT8,
    INT16,
    INT32,
    INT64,
    STRING,
    BOOL,
    FLOAT,
    DOUBLE,
    UNKNOWN
};

// Variable storage locations
enum class VarLocation {
    STACK,
    DATA,
    HEAP
};

// Variable information structure
struct VarInfo {
    string name;
    VarType type;
    string value;
    VarLocation location;
    int stackOffset;
    bool needsCalculation;
    bool hasVariableReference;
    int arraySize;
};

// Function information
struct FunctionInfo {
    string name;
    vector<pair<string, VarType>> parameters;
    vector<string> body;
    VarType returnType;
};

// Global containers
vector<string> codeLines;
vector<string> asmOutput;
map<string, VarInfo> variables;
map<string, FunctionInfo> functions;
vector<string> stringLiterals;
vector<string> dataSection;
vector<string> bssSection;
bool usesPrintf = false;

// Helper function declarations
string trimWhitespace(const string& str);
vector<string> splitString(const string& str, char delimiter);
VarType parseType(const string& typeStr);
string typeToString(VarType type);
VarLocation parseLocation(const string& locStr);
int getTypeSize(VarType type);
string getAsmType(VarType type);
string getRegisterForType(VarType type, int index);
string addStringLiteral(const string& str);
vector<string> parseFunctionParameters(const string& paramStr);
void processPrintfCall(const string& line);
void processFunction(const vector<string>& tokens, const vector<string>& body, size_t& lineIndex);
void processVariableDeclaration(const string& line);
void generateDataSection();
void generateBssSection();
void generateAsmForVariables();
void compileCode();

// Helper function implementations
string trimWhitespace(const string& str) {
    size_t first = str.find_first_not_of(" \t");
    if (string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, (last - first + 1));
}

vector<string> splitString(const string& str, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(str);
    while (getline(tokenStream, token, delimiter)) {
        token = trimWhitespace(token);
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

VarType parseType(const string& typeStr) {
    if (typeStr == "int8") return VarType::INT8;
    if (typeStr == "int16") return VarType::INT16;
    if (typeStr == "int32") return VarType::INT32;
    if (typeStr == "int64") return VarType::INT64;
    if (typeStr == "string") return VarType::STRING;
    if (typeStr == "bool") return VarType::BOOL;
    if (typeStr == "float") return VarType::FLOAT;
    if (typeStr == "double") return VarType::DOUBLE;
    return VarType::UNKNOWN;
}

string typeToString(VarType type) {
    switch (type) {
    case VarType::INT8: return "int8";
    case VarType::INT16: return "int16";
    case VarType::INT32: return "int32";
    case VarType::INT64: return "int64";
    case VarType::STRING: return "string";
    case VarType::BOOL: return "bool";
    case VarType::FLOAT: return "float";
    case VarType::DOUBLE: return "double";
    default: return "unknown";
    }
}

VarLocation parseLocation(const string& locStr) {
    if (locStr == "stack" || locStr == "stk") return VarLocation::STACK;
    if (locStr == "data" || locStr == "dt") return VarLocation::DATA;
    if (locStr == "heap" || locStr == "h") return VarLocation::HEAP;
    return VarLocation::STACK;
}

int getTypeSize(VarType type) {
    switch (type) {
    case VarType::INT8: return 1;
    case VarType::INT16: return 2;
    case VarType::INT32: return 4;
    case VarType::INT64: return 8;
    case VarType::FLOAT: return 4;
    case VarType::DOUBLE: return 8;
    case VarType::STRING: return 8;
    case VarType::BOOL: return 1;
    default: return 0;
    }
}

string getAsmType(VarType type) {
    switch (type) {
    case VarType::INT8: return "byte";
    case VarType::INT16: return "word";
    case VarType::INT32: return "dword";
    case VarType::INT64: return "qword";
    case VarType::FLOAT: return "dword";
    case VarType::DOUBLE: return "qword";
    case VarType::STRING: return "qword";
    case VarType::BOOL: return "byte";
    default: return "unknown";
    }
}

string getRegisterForType(VarType type, int index) {
    if (index >= 6) return "";

    const string intRegs[] = { "rdi", "rsi", "rdx", "rcx", "r8", "r9" };
    const string floatRegs[] = { "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5" };

    if (type == VarType::FLOAT || type == VarType::DOUBLE) {
        return floatRegs[index];
    }
    return intRegs[index];
}

string addStringLiteral(const string& str) {
    string label = "str_" + to_string(stringLiterals.size());
    string content = str;

    // Replace escape sequences
    size_t pos = 0;
    while ((pos = content.find("\\n", pos)) != string::npos) {
        content.replace(pos, 2, "\n");
        pos += 1;
    }

    stringLiterals.push_back(label + " db '" + content + "', 0");
    return label;
}

vector<string> parseFunctionParameters(const string& paramStr) {
    vector<string> params;
    string current;
    bool inType = false;

    for (char c : paramStr) {
        if (c == ':') {
            inType = true;
            current = trimWhitespace(current);
            params.push_back(current);
            current.clear();
        }
        else if (c == ',') {
            if (inType) {
                current = trimWhitespace(current);
                params.push_back(current);
                current.clear();
                inType = false;
            }
            else {
                current += c;
            }
        }
        else {
            current += c;
        }
    }

    if (!current.empty()) {
        current = trimWhitespace(current);
        params.push_back(current);
    }

    return params;
}

void processPrintfCall(const string& line) {
    size_t openParen = line.find('(');
    size_t closeParen = line.rfind(')');

    if (openParen == string::npos || closeParen == string::npos) {
        asmOutput.push_back("; Invalid printf syntax");
        return;
    }

    string content = line.substr(openParen + 1, closeParen - openParen - 1);
    vector<string> parts;
    string current;
    bool inQuotes = false;

    for (char c : content) {
        if (c == '"') {
            inQuotes = !inQuotes;
            current += c;
        }
        else if (c == ',' && !inQuotes) {
            parts.push_back(trimWhitespace(current));
            current.clear();
        }
        else {
            current += c;
        }
    }

    if (!current.empty()) {
        parts.push_back(trimWhitespace(current));
    }

    if (parts.empty()) {
        asmOutput.push_back("; printf requires at least format string");
        return;
    }

    // Process format string
    string formatStr = parts[0];
    if (formatStr.front() == '"' && formatStr.back() == '"') {
        formatStr = formatStr.substr(1, formatStr.length() - 2);
        string formatLabel = "fmt_" + to_string(stringLiterals.size());
        stringLiterals.push_back(formatLabel + " db '" + formatStr + "', 0");

        // Generate assembly
        asmOutput.push_back("    ; printf call");
        asmOutput.push_back("    lea rdi, [" + formatLabel + "]");

        // Process arguments
        int regIndex = 0;
        for (size_t i = 1; i < parts.size(); i++) {
            string arg = parts[i];
            VarType argType = VarType::INT32; // Default to int

            // Try to determine argument type
            if (arg.find('.') != string::npos) {
                argType = VarType::DOUBLE;
            }
            else if (variables.find(arg) != variables.end()) {
                argType = variables[arg].type;
            }

            string reg = getRegisterForType(argType, regIndex);
            if (reg.empty()) {
                // Push to stack for arguments beyond 6
                asmOutput.push_back("    push " + arg);
            }
            else {
                if (argType == VarType::FLOAT || argType == VarType::DOUBLE) {
                    asmOutput.push_back("    movsd " + reg + ", [" + arg + "]");
                }
                else {
                    asmOutput.push_back("    mov " + reg + ", " + arg);
                }
                regIndex++;
            }
        }

        asmOutput.push_back("    xor eax, eax ; For varargs");
        asmOutput.push_back("    call printf");

        // Clean stack if needed
        if (parts.size() > 7) {
            int stackArgs = parts.size() - 7;
            asmOutput.push_back("    add rsp, " + to_string(stackArgs * 8));
        }
    }
    else {
        asmOutput.push_back("; Invalid printf format string");
    }
}

void processFunction(const vector<string>& tokens, const vector<string>& body, size_t& lineIndex) {
    if (tokens.size() < 2) return;

    FunctionInfo func;
    func.name = tokens[1];

    // Parse return type if specified
    if (tokens.size() > 2 && tokens[2] == "->") {
        if (tokens.size() > 3) {
            func.returnType = parseType(tokens[3]);
        }
    }

    // Parse parameters if any
    if (tokens.back().find('(') != string::npos) {
        size_t paramStart = lineIndex;
        while (lineIndex < codeLines.size()) {
            string line = codeLines[lineIndex];
            if (line.find(')') != string::npos) {
                string paramStr = line.substr(line.find('(') + 1);
                paramStr = paramStr.substr(0, paramStr.find(')'));

                vector<string> paramParts = parseFunctionParameters(paramStr);
                for (size_t i = 0; i < paramParts.size(); i += 2) {
                    if (i + 1 < paramParts.size()) {
                        string paramName = paramParts[i];
                        VarType paramType = parseType(paramParts[i + 1]);
                        func.parameters.push_back({ paramName, paramType });
                    }
                }
                break;
            }
            lineIndex++;
        }
    }

    // Process function body
    while (lineIndex < codeLines.size()) {
        string line = codeLines[lineIndex];
        if (trimWhitespace(line) == "end") {
            break;
        }
        func.body.push_back(line);
        lineIndex++;
    }

    functions[func.name] = func;

    // Generate function prologue
    asmOutput.push_back(func.name + ":");
    asmOutput.push_back("    push rbp");
    asmOutput.push_back("    mov rbp, rsp");

    // Allocate space for local variables
    int localVarsSize = 0;
    for (const string& line : func.body) {
        // TODO: Parse local variable declarations
    }

    if (localVarsSize > 0) {
        asmOutput.push_back("    sub rsp, " + to_string(localVarsSize));
    }

    // Process function body
    for (const string& line : func.body) {
        string trimmed = trimWhitespace(line);

        // Skip empty lines and comments
        if (trimmed.empty() || trimmed[0] == ';') {
            continue;
        }

        // Check for printf calls
        if (trimmed.find("printf(") == 0) {
            usesPrintf = true;
            processPrintfCall(trimmed);
            continue;
        }

        // TODO: Process other statements
    }

    // Generate function epilogue
    if (func.returnType != VarType::UNKNOWN) {
        // TODO: Handle return values
    }

    asmOutput.push_back("    leave");
    asmOutput.push_back("    ret");
}

void processVariableDeclaration(const string& line) {
    vector<string> tokens = splitString(line, ' ');
    if (tokens.size() < 3) return;

    VarInfo varInfo;
    varInfo.type = parseType(tokens[0]);

    // Parse variable name and location
    size_t equalPos = line.find('=');
    if (equalPos == string::npos) return;

    string declarationPart = line.substr(0, equalPos);
    vector<string> declTokens = splitString(declarationPart, ' ');

    if (declTokens.size() >= 2 &&
        (declTokens[1] == "stack" || declTokens[1] == "stk" ||
            declTokens[1] == "data" || declTokens[1] == "dt" ||
            declTokens[1] == "heap" || declTokens[1] == "h")) {
        // Has explicit location
        varInfo.name = declTokens[2];
        varInfo.location = parseLocation(declTokens[1]);
    }
    else {
        // Default location (stack)
        varInfo.name = declTokens[1];
        varInfo.location = VarLocation::STACK;
    }

    // Check for array declaration
    size_t bracketPos = varInfo.name.find('[');
    if (bracketPos != string::npos) {
        string arraySizeStr = varInfo.name.substr(bracketPos + 1);
        arraySizeStr = arraySizeStr.substr(0, arraySizeStr.find(']'));
        varInfo.arraySize = stoi(arraySizeStr);
        varInfo.name = varInfo.name.substr(0, bracketPos);
    }
    else {
        varInfo.arraySize = 1;
    }

    // Extract value
    varInfo.value = trimWhitespace(line.substr(equalPos + 1));

    // Special handling for strings
    if (varInfo.type == VarType::STRING) {
        if (varInfo.value.front() == '"' && varInfo.value.back() == '"') {
            string strContent = varInfo.value.substr(1, varInfo.value.length() - 2);
            varInfo.value = addStringLiteral(strContent);
        }
    }

    // Check if value contains operations or variable references
    varInfo.needsCalculation = (varInfo.value.find_first_of("+-*/") != string::npos);
    varInfo.hasVariableReference = false;

    for (char ch : varInfo.value) {
        if (isalpha(ch) && !isdigit(ch)) {
            varInfo.hasVariableReference = true;
            break;
        }
    }

    variables[varInfo.name] = varInfo;
}

void generateDataSection() {
    if (!dataSection.empty()) {
        asmOutput.push_back("section .data");
        for (const string& line : dataSection) {
            asmOutput.push_back(line);
        }
    }

    if (!stringLiterals.empty()) {
        asmOutput.push_back("section .data");
        for (const string& str : stringLiterals) {
            asmOutput.push_back(str);
        }
    }
}

void generateBssSection() {
    if (!bssSection.empty()) {
        asmOutput.push_back("section .bss");
        for (const string& line : bssSection) {
            asmOutput.push_back(line);
        }
    }
}

void generateAsmForVariables() {
    int totalStackSize = 0;

    // Calculate total stack size needed
    for (const auto& varPair : variables) {
        if (varPair.second.location == VarLocation::STACK) {
            totalStackSize += getTypeSize(varPair.second.type) * varPair.second.arraySize;
        }
    }

    // Align stack to 16 bytes
    if (totalStackSize % 16 != 0) {
        totalStackSize += 16 - (totalStackSize % 16);
    }

    // Allocate stack space
    if (totalStackSize > 0) {
        asmOutput.push_back("    sub rsp, " + to_string(totalStackSize));
    }

    // Generate code for each variable
    int stackOffset = 0;
    for (auto& varPair : variables) {
        VarInfo& varInfo = varPair.second;

        if (varInfo.location == VarLocation::STACK) {
            varInfo.stackOffset = stackOffset;
            string asmType = getAsmType(varInfo.type);

            if (varInfo.type == VarType::STRING) {
                asmOutput.push_back("    lea rax, [" + varInfo.value + "]");
                asmOutput.push_back("    mov [rsp+" + to_string(stackOffset) + "], rax");
            }
            else if (!varInfo.needsCalculation && !varInfo.hasVariableReference) {
                asmOutput.push_back("    mov " + asmType + " [rsp+" +
                    to_string(stackOffset) + "], " + varInfo.value);
            }
            else {
                asmOutput.push_back("    ; TODO: Implement expression evaluation for " + varInfo.name);
            }

            stackOffset += getTypeSize(varInfo.type) * varInfo.arraySize;
        }
        else if (varInfo.location == VarLocation::DATA) {
            string asmDirective;
            switch (varInfo.type) {
            case VarType::INT8: asmDirective = "db"; break;
            case VarType::INT16: asmDirective = "dw"; break;
            case VarType::INT32: asmDirective = "dd"; break;
            case VarType::INT64: asmDirective = "dq"; break;
            case VarType::FLOAT: asmDirective = "dd"; break;
            case VarType::DOUBLE: asmDirective = "dq"; break;
            case VarType::STRING: asmDirective = "dq"; break;
            case VarType::BOOL: asmDirective = "db"; break;
            default: asmDirective = "dd"; break;
            }

            if (varInfo.type == VarType::STRING) {
                dataSection.push_back(varInfo.name + " " + asmDirective + " " + varInfo.value);
            }
            else if (!varInfo.needsCalculation && !varInfo.hasVariableReference) {
                if (varInfo.arraySize > 1) {
                    dataSection.push_back(varInfo.name + " times " + to_string(varInfo.arraySize) +
                        " " + asmDirective + " " + varInfo.value);
                }
                else {
                    dataSection.push_back(varInfo.name + " " + asmDirective + " " + varInfo.value);
                }
            }
        }
        else if (varInfo.location == VarLocation::HEAP) {
            asmOutput.push_back("    ; Heap allocation for " + varInfo.name);
            asmOutput.push_back("    mov rdi, " + to_string(getTypeSize(varInfo.type) * varInfo.arraySize));
            asmOutput.push_back("    call malloc");
            asmOutput.push_back("    mov [rsp+" + to_string(stackOffset) + "], rax");

            // Initialize if needed
            if (!varInfo.value.empty() && varInfo.value != "0") {
                asmOutput.push_back("    mov " + getAsmType(varInfo.type) + " [rax], " + varInfo.value);
            }

            stackOffset += 8; // Pointers are 8 bytes
        }
    }
}

void compileCode() {
    // First pass: detect if printf is used
    for (const string& line : codeLines) {
        if (line.find("printf") != string::npos) {
            usesPrintf = true;
            break;
        }
    }

    // Initialize sections
    generateDataSection();
    generateBssSection();

    asmOutput.push_back("section .text");
    asmOutput.push_back("    global _main");
    if (usesPrintf) {
        asmOutput.push_back("    extern printf, malloc, free");
    }
    else {
        asmOutput.push_back("    extern malloc, free");
    }
    asmOutput.push_back("_main:");
    asmOutput.push_back("    push rbp");
    asmOutput.push_back("    mov rbp, rsp");

    // Process variables
    generateAsmForVariables();

    // Process code lines
    for (size_t i = 0; i < codeLines.size(); i++) {
        string line = trimWhitespace(codeLines[i]);
        if (line.empty()) continue;

        vector<string> tokens = splitString(line, ' ');
        if (tokens.empty()) continue;

        // Check for function declaration
        if (tokens[0] == "function") {
            vector<string> body;
            size_t startLine = i;
            while (++i < codeLines.size()) {
                string bodyLine = trimWhitespace(codeLines[i]);
                if (bodyLine == "end") break;
                body.push_back(bodyLine);
            }
            processFunction(tokens, body, startLine);
            continue;
        }

        // Check for printf call
        if (line.find("printf(") == 0) {
            processPrintfCall(line);
            continue;
        }

        // Check for variable declaration
        if (tokens[0] == "int8" || tokens[0] == "int16" || tokens[0] == "int32" ||
            tokens[0] == "int64" || tokens[0] == "float" || tokens[0] == "double" ||
            tokens[0] == "string" || tokens[0] == "bool") {
            processVariableDeclaration(line);
            continue;
        }

        // TODO: Process other statements
    }

    // Add program end
    asmOutput.push_back("    mov rsp, rbp");
    asmOutput.push_back("    pop rbp");
    asmOutput.push_back("    xor eax, eax");
    asmOutput.push_back("    ret");
}

int main() {
    // Read input code
    print("MINI_ASM Compiler (Enter 'RUN' to compile)");
    print("write help for help");
    print("------------------------------------------");

    while (true) {
        printn(">>> ");
        string line;
        input(line);

        if (line == "RUN" || line == "run") {
            break;
        }
        else if (line == "help" || line == "HELP") {
            print("Supports: int8, int16, int32, int64, float, double, string, bool");
            print("Locations: stack, data, heap");
            print("Arrays: int32[10] stack arr = 0");
            print("Functions: function myFunc() ... end");
            print("Printf: printf(\"Hello %d\\n\", 42)");
        }
        else {
            codeLines.push_back(line);
        }
    }

    // Compile the code
    compileCode();

    // Output generated assembly
    print("\nGenerated Assembly:");
    print("------------------");
    for (const string& asmLine : asmOutput) {
        print(asmLine);
    }

    print("\nCompilation completed successfully.");
    return 0;
}

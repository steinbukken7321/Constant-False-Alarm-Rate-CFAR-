/********************************************************
* HEADERS
********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/********************************************************
* PROGRAM DEFINITIONS
********************************************************/
#define T 4e-6 // intervalo de tempo entre símbolos IQ adjacentes no tempo
#define C 2.998e8 // velocidade da luz em metros por segundo

/*****************************************************************************
* DATA TYPE STRUCTURES: 
*****************************************************************************/
typedef enum {
    NO_ERROR,           /* "No error" */
	MEMORY_ERR,         /* "Not enough memory" */
	READ_OPEN_ERR,      /* "Unable to open file for reading" */
	READ_ERR,           /* "Error reading file" */
	WRITE_OPEN_ERR,     /* "Unable to open file for writing" */
	WRITE_ERR,          /* "Error writing to file" */
	BUFFER_ERR,         /* "Unable to attach buffer to file" */
	SIZE_ERR,           /* "Specified size doesn't match data size in file" */
    MAXIT_ERR,          /* "Exceeded maximum number of iterations in function" */
    LOGIC_ERR,          /* "Logic error in function" */
    CMDLINE_ERR,        /* "Incorrect command line argument" */
    MTXDIM_ERR,         /* "Invalid matrix dimensions in file" */
    NROWS_ERR,          /* "Incompatible number of rows between input matrices" */ 
    NCOLS_ERR,          /* "Incompatible number of columns between input matrices" */ 
    UNKNOWN_ERR         /* "Unknown error" */
} ERR;

/********************************************************
* FUNCTION PROTOTYPES
********************************************************/
void Quit(ERR err, const char *name);

/*********************************
* PROTOTIPOS DAS FUNÇÕES MATRIZ_ARD
*********************************/
int getRows(const char *filename);
int getCols(const char *filename);
float *readFromFile(const char *filename, int rows, int cols);
void printMatrix(float *matrix, int rows, int cols);

/**********************************
* PROTOTIPOS DAS FUNÇÕES CFARWindow
**********************************/
int** CFARWindow(int WindowSize, int xFlag, int* vetorSize);
void printVetor(int* vetor, int size);
void imprimirxFlag_minus1(int WindowSize, int vetorSize);
void imprimirxFlag_minus2(int WindowSize, int vetorSize);
void imprimirxFlag_1(int WindowSize, int vetorSize);
void imprimirxFlag_0(int WindowSize, int vetorSize);

/***************************************
*** PROTÓTIPO DA FUNÇÃO CFAR_CA_2D
***************************************/
int CFAR_CA_2D(float *mag, float Threshold, int WindowSize, int MinRange, int MaxRange, const char *filename, int* deltax, int* deltay, int vetorSize, int **LinHit, int **ColHit);


/*****************************************************************************
*** PROTÓTIPOS DAS FUNÇÕES MULTIPLERANGEHITFILTER E MULTIPLEDOPPLERHITFILTER
*****************************************************************************/
int MultipleDopplerHitFilter(float *mag, int *DopplerHits, int *RangeHits, int numHits, const char *filename, int **F1Range, int **F1Doppler);
//int MultipleRangeHitFilter(float *mag, int *DopplerHits, int *RangeHits, int numHits, const char *filename, int **F2Range, int **F2Doppler);


/********************************************************
* MAIN():
********************************************************/
int main() {
    const char *filename = "mag.txt";
    int rows = getRows(filename); // armazena o número de linhas da matriz
    int cols = getCols(filename); // armazena o número de colunas da matriz
    float *matrix = readFromFile(filename, rows, cols); // chama a função readfromfile e lê a matriz do arquivo

    //************************************** Definições manuais **************************************

    int WindowSize = 7;  // Defina o tamanho da janela
    float Threshold = 3.3; // Defina o limiar de decisão
    int MinRange = 0; // Defina o valor mínimo do domínio range [km]
    int MaxRange = 1e5; // Defina o valor máximo do domínio range [km]
	int vetorSize = 0;  // p/ guardar tamanho do vetor
	
    //****************************************** IMPRESSÕES ******************************************

    // 1. Matriz mag
	#if 0
	printf("Matrix (%d x %d):\n", rows, cols);
	printMatrix(matrix, rows, cols);
	#endif
	
	//*****************************************************
    // 2. Janela p/ cada caso de xFlag
    // imprime os valores p/ todos casos de xflag
    #if 0
    // Caso xFlag = -1: Impressão da matriz ∆
    imprimirxFlag_minus1(WindowSize, vetorSize);
	// Caso xFlag = -2: Impressão da ∆ DeltaTransposta
	imprimirxFlag_minus2(WindowSize, vetorSize);
	// Caso xFlag = 1: Transformar ∆ em vetor e exclui os termos 8192
    imprimirxFlag_1(WindowSize, vetorSize);
	// Caso xFlag = 0: Transformar ∆ Transposta e exclui os termos 8192
	imprimirxFlag_0(WindowSize, vetorSize);
	#endif
	
	
	//*****************************************************
	//3.  CFAR_CA_2D
    int* deltax = (int *)CFARWindow(WindowSize, 1, &vetorSize);    // Chamada para CFARWindow com xFlag = 1 para obter deltax
    int* deltay = (int *)CFARWindow(WindowSize, 0, &vetorSize);    // Chamada para CFARWindow com xFlag = 0 para obter deltay
    int *RangeHits = 0;
	int *DopplerHits = 0;
    int NumHits = CFAR_CA_2D(matrix, Threshold, WindowSize, MinRange, MaxRange, filename, deltax, deltay, vetorSize, &RangeHits, &DopplerHits);
	#if 0
    // Imprimir os resultados de RangeHits e DopplerHits
    printf("NumHits: %d\n", NumHits);
    printf("Range =           Doppler =\n");
    for (int i = 0; i < NumHits; i++) {
        printf("%11d      %15d\n", RangeHits[i], DopplerHits[i]);
    } 
	#endif
	
	#if 0  
    // Imprimir os resultados de RangeHits
    printf("Resultados de RangeHits:\n");
    for (int i = 0; i < NumHits; i++) {
        printf("%d\n", RangeHits[i]);
    }

    // Imprimir os resultados de DopplerHits
    printf("Resultados de DopplerHits:\n");
    for (int i = 0; i < NumHits; i++) {
        printf("%d\n", DopplerHits[i]);
    }
	#endif
	
	free(deltax);
    free(deltay);
    
    //*****************************************************
	// 4. CFAR MultipleDopplerHitFilter e MultipleDopplerHitFilter
	#if 0
	int *F1Range = 0;
	int *F1Doppler = 0;
	int F1Resultado = MultipleDopplerHitFilter(matrix, DopplerHits, RangeHits, NumHits, filename, &F1Range, &F1Doppler);

	
	// Imprimir os resultados de RangeHits e DopplerHits após o filtro 1  
	printf("MultipleDopplerFilterHits: \n");
	printf("F1Range =           F1Doppler =\n");
	for (int i = 0; i < F1Resultado; i++) {
    	printf("%11d      %15d\n", F1Range[i], F1Doppler[i]);
	}
	
	free(F1Range);
    free(F1Doppler);
	
	#endif
	
	#if 0
    int *F2Range = 0;
    int *F2Doppler = 0;
    int F2Resultado = MultipleRangeHitFilter(matrix, DopplerHits, RangeHits, NumHits, filename, &F2Range, &F2Doppler);
    
	
    // Imprimir os resultados de RangeHits e DopplerHits após o filtro 2  
    printf("MultipleRangeFilterHits: \n");
    printf("F2Range =           F2Doppler =\n");
    for (int i = 0; i < F2Resultado; i++) {
        printf("%11d      %15d\n", F2Doppler[i], F2Range[i]);
    }
    
    // Liberar memória alocada
    free(F2Range);
    free(F2Doppler);
	#endif





    // Liberar memória alocada para os vetores
    free(RangeHits);
    free(DopplerHits);

    // Liberar a memória alocada para a matriz
    free(matrix);
    return 0;
}

/*************************************************************************
* FUNC: void Quit(ERR err, char *name)
*
* DESC: Prints Error message and quits the program.
**************************************************************************/
void Quit(ERR err, const char *name)
{
	static const char *ErrMess[] = {
	"No error",
	"Not enough memory",
	"Unable to find the file",
	"Error reading file",
	"Unable to open the file",
	"Error writing to file",
	"Unable to attach buffer to file",
	"Specified size doesn't match data size in file",
    "Exceeded maximum number of iterations in function",
    "Logic error in function",
    "Incorrect command line argument",
    "Invalid matrix dimensions in file",
    "Incompatible number of rows between input matrices",
    "Incompatible number of columns between input matrices",
	"Unknown error"
  	};

	system("cls"); // Limpa a tela (Windows)

	if (err != NO_ERROR) {
		if (name[0]) {
			fprintf(stderr, "%s %s!\n", ErrMess[err], name);
		} else {
			fprintf(stderr, "%s!\n", ErrMess[err]);
		}
		exit(err);
	} else {
		exit(0); // Saída sem erro
	}
}


/*************************************************************************
* IMPLEMENTAÇÃO DAS FUNÇÕES MATRIZ_ARD
**************************************************************************/

/********************************************
* FUNC: getRows
* DESC: Conta o número de linhas em um arquivo.
* Parâmetro: filename - nome do arquivo
* Retorno: Número de linhas no arquivo
********************************************/
int getRows(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        Quit(READ_OPEN_ERR, filename);
    }

    int rows = 0;
    int ch;

    // Conta o número de linhas no arquivo
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            rows++;
        }
    }

    fclose(file);
    return rows;
}

/********************************************
* FUNC: getCols
* DESC: Conta o número de colunas na primeira linha de um arquivo.
* Parâmetro: filename - nome do arquivo
* Retorno: Número de colunas na primeira linha do arquivo
********************************************/
int getCols(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        Quit(READ_OPEN_ERR, filename);
    }

    int cols = 0;
    int ch;

    // Conta o número de colunas na primeira linha
    while ((ch = fgetc(file)) != EOF && ch != '\n') {
        if (ch == ' ') {
            cols++;
        }
    }

    fclose(file);
    return cols + 1; 
}

/********************************************
* FUNC: readFromFile
* DESC: Lê uma matriz de valores float de um arquivo.
* Parâmetros: filename - nome do arquivo
*             rows - número de linhas na matriz
*             cols - número de colunas na matriz
* Retorno: Ponteiro para a matriz lida
********************************************/
float *readFromFile(const char *filename, int rows, int cols) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        Quit(READ_OPEN_ERR, filename);
    }

    float *matrix = (float *)malloc(rows * cols * sizeof(float));
    if (!matrix) {
        Quit(MEMORY_ERR, "for matrix allocation");
    }

    // Lê os valores da matriz do arquivo
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (fscanf(file, "%f", &matrix[i * cols + j]) != 1) {
                Quit(READ_ERR, "reading matrix element");
            }
        }
    }

    fclose(file);
    return matrix;
}

/********************************************
* FUNC: printMatrix
* DESC: Imprime uma matriz de valores float.
* Parâmetros: matrix - ponteiro para a matriz
*             rows - número de linhas na matriz
*             cols - número de colunas na matriz
********************************************/
void printMatrix(float *matrix, int rows, int cols) {
    // Imprime a matriz
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%f ", matrix[i * cols + j]);
        }
        printf("\n");
    }
}

/*************************************************************************
* IMPLEMENTAÇÃO DAS FUNÇÕES CFARWindow
**************************************************************************/

/********************************************
* FUNC: CFARWindow
* --------------------
* DESC: Cria uma janela CFAR (Constant False Alarm Rate) com base no tamanho fornecido.
*
* Parâmetros: WindowSize: O tamanho da janela, que deve ser um número ímpar entre 5 e 8192.
* xFlag: Um indicador utilizado para determinar o tipo de operação:
*        - xFlag = -2: Retorna a matriz transposta de ∆ onde as células de referência armazenam valores de deslocamento delta do índice y.
*                      As posições correspondentes à CUT e às células do anel de guarda armazenam o valor 8192.
*        - xFlag = -1: Retorna a matriz ∆ onde as células de referência armazenam valores de deslocamento delta do índice x.
*                      As posições correspondentes à CUT e às células do anel de guarda armazenam o valor 8192.
*        - xFlag = 1: Transforma a matriz ∆ em um vetor e e retorna oos valores do vetor (as células 8192 desaparecem).
*        - xFlag = 0: Transforma a matriz transposta de ∆ em um vetor e retorna os valores do vetor transposto (as células 8192 desaparecem).
* vetorSize: ponteiro para um inteiro que será preenchido com o tamanho do vetor retornado. Este parâmetro só é utilizado quando xFlag é 0 ou 1.
*            Indica o número de elementos válidos (diferentes de 8192) no vetor retornado.
********************************************/
int** CFARWindow(int WindowSize, int xFlag, int* vetorSize) {
    // Verificar se o tamanho da janela é ímpar e está no intervalo permitido
    if (WindowSize % 2 == 0 || WindowSize < 5 || WindowSize >= 8192) {
        return NULL;
    }

    int N = (WindowSize - 1) / 2;

    // Alocar matriz Delta
    int** Delta = (int**)malloc(WindowSize * sizeof(int*));
    for (int i = 0; i < WindowSize; i++) {
        Delta[i] = (int*)malloc(WindowSize * sizeof(int));
    }

    // Preencher a matriz Delta
    for (int col = 0; col < WindowSize; col++) {
        int v_col = -N + col;
        for (int row = 0; row < WindowSize; row++) {
            Delta[row][col] = v_col;
        }
    }

    // Ajustar o CUT e o anel de guarda
    for (int row = N - 1; row <= N + 1; row++) {
        for (int col = N - 1; col <= N + 1; col++) {
            Delta[row][col] = 8192;
        }
    }
    if (xFlag == -2) {
        // Retornar matriz transposta
        int** DeltaTransposta = (int**)malloc(WindowSize * sizeof(int*));
        for (int i = 0; i < WindowSize; i++) {
            DeltaTransposta[i] = (int*)malloc(WindowSize * sizeof(int));
        }
        for (int i = 0; i < WindowSize; i++) {
            for (int j = 0; j < WindowSize; j++) {
                DeltaTransposta[i][j] = Delta[j][i];
            }
        }
        return DeltaTransposta;
    } else if (xFlag == -1) {
        // Retornar matriz Delta
        return Delta;  
    } else if (xFlag == 0 || xFlag == 1) {
        *vetorSize = 0;
        for (int i = 0; i < WindowSize; i++) {
            for (int j = 0; j < WindowSize; j++) {
                if (Delta[i][j] != 8192) {
                    (*vetorSize)++;
                }
            }
        }

        int* vetor = (int*)malloc(*vetorSize * sizeof(int));
        int index = 0;

        if (xFlag == 0) {
            // Transformar matriz transposta em vetor
            for (int i = 0; i < WindowSize; i++) {
                for (int j = 0; j < WindowSize; j++) {
                    if (Delta[j][i] != 8192) {
                        vetor[index++] = Delta[j][i];
                    }
                }
            }
        } else {
            // Transformar matriz em vetor
            for (int i = 0; i < WindowSize; i++) {
                for (int j = 0; j < WindowSize; j++) {
                    if (Delta[i][j] != 8192) {
                        vetor[index++] = Delta[i][j];
                    }
                }
            }
        }

        // Libera a memória alocada para a matriz Delta
        for (int i = 0; i < WindowSize; i++) {
            free(Delta[i]);
        }
        free(Delta);

        // Retornar o vetor
        return (int**)vetor;
    } else {
    // xFlag não suportado
    return NULL;
	}
}

/********************************************
* FUNCTION: imprimirxFlag_minus1
* --------------------
* DESC: imprime a janela para xFlag = -1, retornando a matriz ∆
*
* Parâmetros: WindowSize: O tamanho da janela, que deve ser um número ímpar entre 5 e 8192.
* 			  VetorSize: Não é utilizado
* OBS:
* xFlag: Um indicador utilizado para determinar o tipo de operação:
*        - xFlag = -2: Retorna a matriz transposta de ∆ onde as células de referência armazenam valores de deslocamento delta do índice y.
*                      As posições correspondentes à CUT e às células do anel de guarda armazenam o valor 8192.
*        - xFlag = -1: Retorna a matriz ∆ onde as células de referência armazenam valores de deslocamento delta do índice x.
*                      As posições correspondentes à CUT e às células do anel de guarda armazenam o valor 8192.
*        - xFlag = 1: Transforma a matriz ∆ em um vetor e e retorna oos valores do vetor (as células 8192 desaparecem).
*        - xFlag = 0: Transforma a matriz transposta de ∆ em um vetor e retorna os valores do vetor transposto (as células 8192 desaparecem).
* 
********************************************/
void imprimirxFlag_minus1(int WindowSize, int vetorSize) {
    // Obter a matriz Delta usando CFARWindow com xFlag = -1
    int **Delta = CFARWindow(WindowSize, -1, &vetorSize);
    
    printf("CFARWindow(%d, %d):\n", WindowSize, -1);
    if (Delta != NULL) {
        // Imprimir a matriz Delta
        for (int i = 0; i < WindowSize; i++) {
            for (int j = 0; j < WindowSize; j++) {
                printf("%5d ", Delta[i][j]);
            }
            printf("\n");
        }
        
        // Liberar memória alocada para Delta
        for (int i = 0; i < WindowSize; i++) {
            free(Delta[i]);
        }
        free(Delta);
    } else {
        printf("Erro ao criar a janela CFAR para xFlag = -1.\n");
    }
    
    printf("\n");
}

/********************************************
* FUNCTION: imprimirxFlag_minus2
* --------------------
* DESC: imprime a janela para xFlag = -2, retornando a matriz ∆ transposta
*
* Parâmetros: WindowSize: O tamanho da janela, que deve ser um número ímpar entre 5 e 8192.
* 			  VetorSize: Não é utilizado
* OBS:
* xFlag: Um indicador utilizado para determinar o tipo de operação:
*        - xFlag = -2: Retorna a matriz transposta de ∆ onde as células de referência armazenam valores de deslocamento delta do índice y.
*                      As posições correspondentes à CUT e às células do anel de guarda armazenam o valor 8192.
*        - xFlag = -1: Retorna a matriz ∆ onde as células de referência armazenam valores de deslocamento delta do índice x.
*                      As posições correspondentes à CUT e às células do anel de guarda armazenam o valor 8192.
*        - xFlag = 1: Transforma a matriz ∆ em um vetor e e retorna oos valores do vetor (as células 8192 desaparecem).
*        - xFlag = 0: Transforma a matriz transposta de ∆ em um vetor e retorna os valores do vetor transposto (as células 8192 desaparecem).
* 
********************************************/
void imprimirxFlag_minus2(int WindowSize, int vetorSize) {
    // Obter a matriz DeltaTransposta usando CFARWindow com xFlag = -2
    int **DeltaTransposta = CFARWindow(WindowSize, -2, &vetorSize);
    
    printf("CFARWindow(%d, %d):\n", WindowSize, -2);
    if (DeltaTransposta != NULL) {
        // Imprimir a matriz DeltaTransposta
        for (int i = 0; i < WindowSize; i++) {
            for (int j = 0; j < WindowSize; j++) {
                printf("%5d ", DeltaTransposta[i][j]);
            }
            printf("\n");
        }
        
        // Liberar memória alocada para DeltaTransposta
        for (int i = 0; i < WindowSize; i++) {
            free(DeltaTransposta[i]);
        }
        free(DeltaTransposta);
    } else {
        printf("Erro ao criar a janela CFAR para xFlag = -2.\n");
    }
    
    printf("\n");
}

/********************************************
* FUNCTION: imprimirxFlag_1
* --------------------
* DESC: xFlag = 1, transforma a matriz ∆ em um vetor e imprime o vetor
*
* Parâmetros: WindowSize: O tamanho da janela, que deve ser um número ímpar entre 5 e 8192.
* 			  VetorSize: ponteiro para um inteiro que será preenchido com o tamanho do vetor retornado.
* OBS:
* xFlag: Um indicador utilizado para determinar o tipo de operação:
*        - xFlag = -2: Retorna a matriz transposta de ∆ onde as células de referência armazenam valores de deslocamento delta do índice y.
*                      As posições correspondentes à CUT e às células do anel de guarda armazenam o valor 8192.
*        - xFlag = -1: Retorna a matriz ∆ onde as células de referência armazenam valores de deslocamento delta do índice x.
*                      As posições correspondentes à CUT e às células do anel de guarda armazenam o valor 8192.
*        - xFlag = 1: Transforma a matriz ∆ em um vetor e e retorna oos valores do vetor (as células 8192 desaparecem).
*        - xFlag = 0: Transforma a matriz transposta de ∆ em um vetor e retorna os valores do vetor transposto (as células 8192 desaparecem).
* 
********************************************/
void imprimirxFlag_1(int WindowSize, int vetorSize) {
    // Obter o vetor Delta usando CFARWindow com xFlag = 1
    int *vetorDelta = (int *)CFARWindow(WindowSize, 1, &vetorSize);
    
    printf("CFARWindow(%d, %d):\n", WindowSize, 1);
    if (vetorDelta != NULL) {
        // Imprimir o vetor Delta
        printVetor(vetorDelta, vetorSize);
        
        // Liberar memória alocada para vetorDelta
        free(vetorDelta);
    } else {
        printf("Erro ao criar a janela CFAR para xFlag = 1.\n");
    }
    
    printf("\n");
}

/********************************************
* FUNCTION: imprimirxFlag_0
* --------------------
* DESC: xFlag = 0, transforma a matriz ∆ transposta em um vetor e imprime o vetor
*
* Parâmetros: WindowSize: O tamanho da janela, que deve ser um número ímpar entre 5 e 8192.
* 			  VetorSize: ponteiro para um inteiro que será preenchido com o tamanho do vetor retornado.
* OBS:
* xFlag: Um indicador utilizado para determinar o tipo de operação:
*        - xFlag = -2: Retorna a matriz transposta de ∆ onde as células de referência armazenam valores de deslocamento delta do índice y.
*                      As posições correspondentes à CUT e às células do anel de guarda armazenam o valor 8192.
*        - xFlag = -1: Retorna a matriz ∆ onde as células de referência armazenam valores de deslocamento delta do índice x.
*                      As posições correspondentes à CUT e às células do anel de guarda armazenam o valor 8192.
*        - xFlag = 1: Transforma a matriz ∆ em um vetor e e retorna oos valores do vetor (as células 8192 desaparecem).
*        - xFlag = 0: Transforma a matriz transposta de ∆ em um vetor e retorna os valores do vetor transposto (as células 8192 desaparecem).
* 
********************************************/
void imprimirxFlag_0(int WindowSize, int vetorSize) {
    // Obter o vetor usando CFARWindow com xFlag = 0
    int *vetorDeltaTransposta = (int *)CFARWindow(WindowSize, 0, &vetorSize);
    
    printf("CFARWindow(%d, %d):\n", WindowSize, 0);
    if (vetorDeltaTransposta != NULL) {
        // Imprimir o vetor
        printVetor(vetorDeltaTransposta, vetorSize);
        
        // Liberar memória alocada para vetorDeltaTransposta
        free(vetorDeltaTransposta);
    } else {
        printf("Erro ao criar a janela CFAR para xFlag = 0.\n");
    }
    
    printf("\n");
}

/********************************************************
* IMPLEMENTAÇÃO DAS FUNÇÃO PRINTVETOR
********************************************************/
/********************************************
* FUNCTION: printVetor
* --------------------
* DESC: Imprime os valores de um vetor.
*
* Parâmetros: vetor: Ponteiro para o vetor a ser impresso.
* 			  size: Tamanho do vetor.
* retorno: impressão do vetor
********************************************/
void printVetor(int* vetor, int size) {
    // Verifica se o ponteiro é nulo
    if (vetor == NULL) {
        Quit(LOGIC_ERR, "Null pointer passed to printVetor");
    }

    // Imprime os valores do vetor
    for (int i = 0; i < size; i++) {
        printf("%5d ", vetor[i]);
    }
    printf("\n");
}

/********************************************************
* IMPLEMENTAÇÃO DA FUNC CFAR_CA_2D
********************************************************/
/********************************************
* FUNC: CFAR_CA_2D
* --------------------
* DESC:  Implementação da função CFAR_CA_2D para detecção de alvos em matrizes 2D de magnitude.
*   
*	Parâmetros:
*   	mag: Matriz de magnitude Ψ no domínio Doppler e Range, de dimensão [NumDoppler, NumRangePoints].
*   	Threshold: Valor em ponto flutuante na faixa [2.0, 20.0] que define o limiar de decisão do CFAR.
*   	WindowSize: Tamanho da janela de CFAR no domínio Range. WindowSize deve ser um inteiro ímpar igual ou maior que 5.
*   	MinRange e MaxRange: Delimitação do domínio Range no mov da janela W(range,v) sobre o dominio (range,v) de Ψ.
*   	filename: chama o arquivo filename e aplica as funções auxiliares
*		deltax e deltay: - (deltax) xFlag = 1: Transforma a matriz ∆ em um vetor e e retorna oos valores do vetor.
*        			 	 - (deltay) xFlag = 0: Transforma a matriz transposta de ∆ em um vetor e retorna os valores do vetor transposto.
*		vetorSize: Tamanho dos vetores deltax e deltay, que representa o número de elementos nesses vetores.
* Nota: a função CFAR_CA_2D() chama a função CFARWindow(), cujo código foi descrito acima.
*   
* RETORNO DA FUNC:
*   LinHit(RangeHits): Vetor contendo os índices dos hits do CFAR no domínio Doppler v da matriz Ψ.
*   ColHit(DopplerHits): Vetor contendo os índices dos hits do CFAR no domínio Range da matriz Ψ.
*   NumHits: Indica o número total de hits resultantes do CFAR

* Nota: LinHit(RangeHits) e ColHit(DopplerHits) são vetores com o mesmo número NumHits de elementos, 
********************************************/
int CFAR_CA_2D(float *mag, float Threshold, int WindowSize, int MinRange, int MaxRange, const char *filename, int* deltax, int* deltay, int vetorSize, int **RangeHits, int **DopplerHits) {
    int NRanges = getCols(filename);
    int NFDoppl = getRows(filename);

    MinRange = round(MinRange / (T * C));
    MaxRange = round(MaxRange / (T * C));

    int Ndelta = vetorSize;

    // Inicialização das variáveis para armazenar os hits
    int maxHits = 1000;  // Número máximo esperado de hits
    int NumHits = 0;
    *RangeHits = (int *)malloc(maxHits * sizeof(int));
    *DopplerHits = (int *)malloc(maxHits * sizeof(int));

    // Verificar se a alocação de memória foi bem-sucedida
    if (*RangeHits == NULL || *DopplerHits == NULL) {
        Quit(MEMORY_ERR, "for hit vectors allocation");
    }

    // Algoritmo CFAR_CA_2D para detecção de hits
    for (int row = WindowSize; row < NFDoppl - WindowSize; row++) {
        for (int col = WindowSize; col < NRanges - WindowSize; col++) {
            if (col < MinRange || col > MaxRange) {
                continue;
            }

            float u = 0.0;
            for (int n = 0; n < Ndelta; n++) {
                u += (1.0 / Ndelta) * mag[((row + deltay[n]) * NRanges) + (col + deltax[n])];
            }

            if (mag[row * NRanges + col] > u * Threshold) {
                // Verificar se há espaço suficiente nos vetores
                if (NumHits >= maxHits) {
                    // Realocar os vetores para aumentar a capacidade
                    maxHits *= 2;  // Dobrar a capacidade de hits máximos
                    *RangeHits = (int *)realloc(*RangeHits, maxHits * sizeof(int));
                    *DopplerHits = (int *)realloc(*DopplerHits, maxHits * sizeof(int));
                    if (*RangeHits == NULL || *DopplerHits == NULL) {
                        free(*RangeHits);
                        free(*DopplerHits);
                        Quit(MEMORY_ERR, "for hit vectors reallocation");
                    }
                }

                // Armazenar os hits detectados
                (*RangeHits)[NumHits] = col;
                (*DopplerHits)[NumHits] = row;
                NumHits++;
            }
        }
    }
    return NumHits; // Retorna o número de hits encontrados
}

/********************************************************
* IMPLEMENTAÇÃO DAS FUNÇÕES DOS FILTROS
********************************************************/
/********************************************
*FUNC: MultipleDopplerHitFilter
*--------------------
*DESC: Implementação da função MultipleDopplerHitFilter para filtrar hits Doppler em matrizes de magnitude.
*	Parâmetros:
*		mag: Ponteiro para a matriz de magnitude.
*		DopplerHits: Vetor de hits Doppler.
*		RangeHits: Vetor de hits de range.
*		numHits: Número total de hits não filtrados.
*		filename: Nome do arquivo usado para obter o número de colunas da matriz de magnitude.
*		F1Range: Ponteiro para a matriz filtrada de hits de range.
*		F1Doppler: Ponteiro para a matriz filtrada de hits Doppler.
*
*	Retornos:
*		F1Range:  Matriz filtrada contendo os hits de alcance.
*		F1Doppler: Matriz filtrada contendo os hits Doppler
*		numValidResultsF1: Número de resultados válidos após a filtragem.
*********************************************/
int MultipleDopplerHitFilter(float *mag, int *DopplerHits, int *RangeHits, int numHits, const char *filename, int **F1Range, int **F1Doppler) {
    // número total de hits não filtrados
    int NUnfilteredHits = numHits;
    
    // indices para a linha (GrIdx) e coluna (ElIdx) durante a iteração
    int ElIdx = 0;
    int GrIdx = 0;

    // número de colunas na matriz de magnitude (mag) obtido da função getCols
    int Cols = getCols(filename);

    // primeira passagem para calcular o número de linhas (GrIdx) e o número máximo de colunas (ElIdx)
    for (int n = 0; n <= NUnfilteredHits - 2; n++) {
        // se o valor de DopplerHits no índice atual for igual ao próximo, incrementa o índice da coluna
        if (DopplerHits[n] == DopplerHits[n + 1]) {
            ElIdx++;
        } else {
            // se o valor de DopplerHits mudar, incrementa o índice da linha e reseta o índice da coluna
            GrIdx++;
            ElIdx = 0;
        }
    }
    // incrementar GrIdx para incluir o último grupo
    GrIdx++;

    // armazenar os valores de GrIdx e ElIdx como número de linhas e colunas para as matrizes F1Range e F1Doppler
    int linhas = GrIdx;
    int colunas = ElIdx + 1; // Adiciona 1 porque o índice começa em 0

    // alocar memória para F1Range e F1Doppler com os tamanhos calculados
    *F1Range = (int *)calloc(linhas * colunas, sizeof(int));
    *F1Doppler = (int *)calloc(linhas * colunas, sizeof(int));

    // verificar se a alocação de memória foi bem-sucedida
    if (*F1Range == NULL || *F1Doppler == NULL) {
        Quit(MEMORY_ERR, "for hit vectors allocation");
    }

    // redefinir os índices para preencher as matrizes com os dados
    ElIdx = 0;
    GrIdx = 0;
    
    // preencher F1Range e F1Doppler com os valores dos hits
    for (int n = 0; n <= NUnfilteredHits - 2; n++) {
        	(*F1Range)[GrIdx * colunas + ElIdx] = RangeHits[n];
        	(*F1Doppler)[GrIdx * colunas + ElIdx] = DopplerHits[n];
        	// se o valor de DopplerHits mudar, incrementa o índice da linha e reseta o índice da coluna
        	if (DopplerHits[n] == DopplerHits[n + 1]) {
            	ElIdx++;
        } else {
            GrIdx++;
            ElIdx = 0;
        }
    }
    
    // copiando o último valor do array para F1Range e F1Doppler
    (*F1Range)[GrIdx * colunas + ElIdx] = RangeHits[NUnfilteredHits - 1];
    (*F1Doppler)[GrIdx * colunas + ElIdx] = DopplerHits[NUnfilteredHits - 1];

    // p/ alocar memória para armazenar os máximos encontrados
    int *F1_RangeHits = (int *)calloc(linhas, sizeof(int));
    int *F1_DopplerHits = (int *)calloc(linhas, sizeof(int));

    // p/ verificar se a alocação de memória foi bem-sucedida
    if (F1_RangeHits == NULL || F1_DopplerHits == NULL) {
        Quit(MEMORY_ERR, "for hit vectors allocation");
    }

    // encontrando os máximos valores para cada linha (grupo)
    for (GrIdx = 0; GrIdx <= linhas-1; GrIdx++) {
        float maxval = 0.0; // Inicializa o valor máximo como zero
        unsigned IdxMax = 0; // Inicializa o índice do valor máximo
        
        // Percorrer as colunas da linha atual para encontrar o valor máximo
        for (ElIdx = 0; ElIdx <= colunas-1; ElIdx++) {
            // Verificar se o valor de DopplerHits não é zero
            if ((*F1Doppler)[GrIdx * colunas + ElIdx] != 0) {
                int Idx1 = (*F1Range)[GrIdx * colunas + ElIdx];
                int Idx2 = (*F1Doppler)[GrIdx * colunas + ElIdx];

                // Atualizar o valor máximo e o índice do valor máximo
                if (mag[Idx2 * Cols + Idx1] > maxval) {
                    maxval = mag[Idx2 * Cols + Idx1];
                    IdxMax = ElIdx;
                }
            }
        }
        // P/ armazenar os valores máximos encontrados para a linha atual
        F1_RangeHits[GrIdx] = (*F1Range)[GrIdx * colunas + IdxMax];
        F1_DopplerHits[GrIdx] = (*F1Doppler)[GrIdx * colunas + IdxMax];
    }

	// p/ adicionar a variável para armazenar o número de resultados válidos
    int numValidResultsF1 = linhas;
    // Liberar a memória temporária utilizada para armazenar os dados intermediários
    free(*F1Range);
    free(*F1Doppler);
    


    // p/ atribuir os resultados finais para F1Range e F1Doppler
    *F1Range = F1_RangeHits;
    *F1Doppler = F1_DopplerHits;
    #if 0
    // Imprimir valores finais de F1Range e F1Doppler
    printf("F1Range final:\n");
    for (int i = 0; i < numValidResultsF1; i++) {
        printf("%d ", (*F1Range)[i]);
    }
    printf("\n");

    printf("F1Doppler final:\n");
    for (int i = 0; i < numValidResultsF1; i++) {
        printf("%d ", (*F1Doppler)[i]);
    }
    printf("\n");
    #endif
    return numValidResultsF1;
}





/********************************************
*FUNC: MultipleRangeHitFilter
*--------------------
*DESC: Implementação da função MultipleRangeHitFilter para filtrar hits de alcance em matrizes de magnitude.
*	Parâmetros:
*		mag: Ponteiro para a matriz de magnitude.
*		RangeHits: Vetor de hits de alcance.
*		DopplerHits: Vetor de hits Doppler.
*		numHits: Número total de hits não filtrados.
*		filename: Nome do arquivo usado para obter o número de colunas da matriz de magnitude.
*		F2Range: Ponteiro para a matriz filtrada de hits de alcance.
*		F2Doppler: Ponteiro para a matriz filtrada de hits Doppler.
*
*	Retornos:
*		F2Range:  Matriz filtrada contendo os hits de alcance.
*		F2Doppler: Matriz filtrada contendo os hits Doppler.
*		numValidResultsF2: Número de resultados válidos após a filtragem.
*********************************************/
/**
int MultipleRangeHitFilter(float *mag, int *RangeHits, int *DopplerHits, int numHits, const char *filename, int **F2Range, int **F2Doppler) {
    // número total de hits não filtrados
    int NUnfilteredHits = numHits;
    
    // índices para a linha (GrIdx) e coluna (ElIdx) durante a iteração
    int ElIdx = 0;
    int GrIdx = 0;

    // número de colunas na matriz de magnitude (mag) obtido da função getCols
    int Cols = getCols(filename);
	#if 1
	// Imprimir valores de RangeHits e DopplerHits antes do filtro
    printf("RangeHits antes do filtro:\n");
    for (int i = 0; i < NUnfilteredHits; i++) {
        printf("%d ", RangeHits[i]);
    }
    printf("\n");

    printf("DopplerHits antes do filtro:\n");
    for (int i = 0; i < NUnfilteredHits; i++) {
        printf("%d ", DopplerHits[i]);
    }
    printf("\n");
    #endif

    // primeira passagem para calcular o número de linhas (GrIdx) e o número máximo de colunas (ElIdx)
    for (int n = 0; n <= NUnfilteredHits - 2; n++) {
        // se o valor de RangeHits no índice atual for igual ao próximo, incrementa o índice da coluna
        if (RangeHits[n] == RangeHits[n + 1]) {
            ElIdx++;
        } else {
            // se o valor de RangeHits mudar, incrementa o índice da linha e reseta o índice da coluna
            GrIdx++;
            ElIdx = 0;
        }
    }
    // incrementar GrIdx para incluir o último grupo
    GrIdx++;

    // armazenar os valores de GrIdx e ElIdx como número de linhas e colunas para as matrizes F2Range e F2Doppler
    int linhas = GrIdx;
    int colunas = ElIdx + 1; // Adiciona 1 porque o índice começa em 0

    // alocar memória para F2Range e F2Doppler com os tamanhos calculados
    *F2Range = (int *)calloc(linhas * colunas, sizeof(int));
    *F2Doppler = (int *)calloc(linhas * colunas, sizeof(int));

    // verificar se a alocação de memória foi bem-sucedida
    if (*F2Range == NULL || *F2Doppler == NULL) {
        Quit(MEMORY_ERR, "for hit vectors allocation");
    }

    // redefinir os índices para preencher as matrizes com os dados
    ElIdx = 0;
    GrIdx = 0;
    
    // preencher F2Range e F2Doppler com os valores dos hits
    for (int n = 0; n <= NUnfilteredHits - 2; n++) {
        (*F2Range)[GrIdx * colunas + ElIdx] = RangeHits[n];
        (*F2Doppler)[GrIdx * colunas + ElIdx] = DopplerHits[n];
        // se o valor de RangeHits mudar, incrementa o índice da linha e reseta o índice da coluna
        if (RangeHits[n] == RangeHits[n + 1]) {
            ElIdx++;
        } else {
            GrIdx++;
            ElIdx = 0;
        }
    }
    
    // copiando o último valor do array para F2Range e F2Doppler
    (*F2Range)[GrIdx * linhas + ElIdx] = RangeHits[NUnfilteredHits - 1];
    (*F2Doppler)[GrIdx * colunas + ElIdx] = DopplerHits[NUnfilteredHits - 1];

    // p/ alocar memória para armazenar os máximos encontrados
    int *F2_RangeHits = (int *)calloc(linhas, sizeof(int));
    int *F2_DopplerHits = (int *)calloc(linhas, sizeof(int));

    // p/ verificar se a alocação de memória foi bem-sucedida
    if (F2_RangeHits == NULL || F2_DopplerHits == NULL) {
        Quit(MEMORY_ERR, "for hit vectors allocation");
    }

    // encontrando os máximos valores para cada linha (grupo)
    for (GrIdx = 0; GrIdx <= (colunas - 1); GrIdx++) {
        float maxval = 0.0; // Inicializa o valor máximo como zero
        unsigned IdxMax = 0; // Inicializa o índice do valor máximo
        
        // Percorrer as colunas da linha atual para encontrar o valor máximo
        for (ElIdx = 0; ElIdx <= (linhas - 1); ElIdx++) {
            // Verificar se o valor de RangeHits não é zero
            if ((*F2Range)[GrIdx * colunas + ElIdx] != 0) {
                int Idx1 = (*F2Range)[GrIdx * colunas + ElIdx];
                int Idx2 = (*F2Doppler)[GrIdx * colunas + ElIdx];

                // Atualizar o valor máximo e o índice do valor máximo
                if (mag[Idx1 * Cols + Idx2] > maxval) {
                    maxval = mag[Idx2 * Cols + Idx1];
                    IdxMax = ElIdx;
                }
            }
        }
        // P/ armazenar os valores máximos encontrados para a linha atual
        F2_RangeHits[GrIdx] = (*F2Range)[GrIdx * colunas + IdxMax];
        F2_DopplerHits[GrIdx] = (*F2Doppler)[GrIdx * colunas + IdxMax];
    }

    // p/ adicionar a variável para armazenar o número de resultados válidos
    int numValidResultsF2 = colunas;
    // Liberar a memória temporária utilizada para armazenar os dados intermediários
    free(*F2Range);
    free(*F2Doppler);

    // p/ atribuir os resultados finais para F2Range e F2Doppler
    *F2Range = F2_RangeHits;
    *F2Doppler = F2_DopplerHits;

    #if 1
    // Imprimir valores finais de F2Range e F2Doppler
    printf("F2Range final:\n");
    for (int i = 0; i < numValidResultsF2; i++) {
        printf("%d ", (*F2Range)[i]);
    }
    printf("\n");

    printf("F2Doppler final:\n");
    for (int i = 0; i < numValidResultsF2; i++) {
        printf("%d ", (*F2Doppler)[i]);
    }
    printf("\n");
    #endif

    return numValidResultsF2;
}

*/
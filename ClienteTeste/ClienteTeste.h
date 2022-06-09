#pragma once
#define TAM 100
#define NOMEMP TEXT("MP")

//MONITOR-SERVIDOR
#define MS_WRITE_NAME TEXT("MS_WRITE")
#define MS_READ_NAME TEXT("MS_READ")

//SERVIDOR-MONITOR
#define SM_WRITE_NAME TEXT("SM_WRITE")
#define SM_READ_NAME TEXT("SM_READ")

#define MUTEX_NAME TEXT("MUTEX_SERVIDOR")

#define BUFFER_SIZE 10

#define PIPE_NAME TEXT("\\\\.\\pipe\\clientes")

enum Direction { UP, DOWN, LEFT, RIGHT, NOTHING };
enum PATH { START, END, NORMAL };

typedef struct {
    int cano_pos; //numero do cano escolhido
    enum Direction entrada_possiveis[2];//sendo 0 entrada 1 e saida, 
    //inicialmente não se sabe mas á medida que a agua ai fluindo ai se sabendo
    enum PATH path;//se é o inicio o fim ou um espaço normal
    int agua;//se ja foi prenchido com agua 1 = prenchido, 0 = não prenchido
}CANO;

typedef struct {
    int numero;
    TCHAR nome[TAM];
}CLIENTE_PIPE;


typedef struct {
    unsigned int shutdown; //terminar jogo
    int code; //codigo que indica estado do jogo para imprimir
    boolean modoAleatorio; //indica se as peças vem aleatórias ou não
    int tam_x; //dimensão x maxima matriz
    int tam_y; //dimensão y maxima matriz
    int tempo_fluir; //tempo após o qual a água começa a fluir
    int sp_x; // x da starting position
    int sp_y; // y da starting position
    int ep_x; // x da ending position
    int ep_y; // y da ending position
    int agua_posY; //pos da agua y
    int agua_posX; //pos da agua x
    CANO mapa[20][20]; // mapa do jogo
}DADOS_JOGO;

typedef struct {
    int numero;
    TCHAR nome[TAM];
    DADOS_JOGO dados_jogo;
}CLIENTE;


typedef struct _SharedMem {
    int m; //numero de monitores

    DADOS_JOGO Dados_Partilhados;//memoria partilhada do jogo

    TCHAR comandos[10][BUFFER_SIZE];// buffer circular para receber comandos

}SharedMem;

typedef struct _ControlData {
    HANDLE hMutex;// mutex

    //SEMÁFOROS
    //monitor-servidor
    HANDLE hWriteMS;// semaforo para escrever MS
    HANDLE hReadMS;// semaforo para ler MS

    //servidor-monitor
    HANDLE hWriteSM;// semaforo para escrever SM
    HANDLE hReadSM;// semaforo para ler SM

    unsigned int escreverPos;//posição a escrever
    unsigned int lerPos;//posição a ler

    HANDLE hThreads[2];//threads

    SharedMem* sharedMem;//memoria partilhada
    DADOS_JOGO* dados; //dados do jogo(para evitar chamar muitos parametros)
}ControlData;
#define TAM 256
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
    //inicialmente n�o se sabe mas � medida que a agua ai fluindo ai se sabendo
    enum PATH path;//se � o inicio o fim ou um espa�o normal
    int agua;//se ja foi prenchido com agua 1 = prenchido, 0 = n�o prenchido
}CANO;


typedef struct {
    unsigned int shutdown; //terminar jogo
    int code; //codigo que indica estado do jogo para imprimir
    boolean modoAleatorio; //indica se as pe�as vem aleat�rias ou n�o
    int tam_x; //dimens�o x maxima matriz
    int tam_y; //dimens�o y maxima matriz
    int tempo_fluir; //tempo ap�s o qual a �gua come�a a fluir
    int sp_x; // x da starting position
    int sp_y; // y da starting position
    int ep_x; // x da ending position
    int ep_y; // y da ending position
    int agua_posY; //pos da agua y
    int agua_posX; //pos da agua x
    int jogadas[6];
    CANO mapa[20][20]; // mapa do jogo
}DADOS_JOGO;

typedef struct {
    DWORD ID;
    int numero;
    TCHAR nome[TAM];
    DADOS_JOGO dados_jogo;
    BOOL ativo;
}CLIENTE;


typedef struct _ControlData {
    HANDLE hMutex;// mutex

    //SEM�FOROS
    //monitor-servidor
    HANDLE hWriteMS;// semaforo para escrever MS
    HANDLE hReadMS;// semaforo para ler MS

    //servidor-monitor
    HANDLE hWriteSM;// semaforo para escrever SM
    HANDLE hReadSM;// semaforo para ler SM

    unsigned int escreverPos;//posi��o a escrever
    unsigned int lerPos;//posi��o a ler

    HANDLE hThreads[2];//threads

    CLIENTE* cliente;

    HANDLE* pipe;

    int shutdown;

    //DADOS_JOGO* dados; //dados do jogo(para evitar chamar muitos parametros)
}ControlData;
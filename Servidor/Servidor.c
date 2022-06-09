#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "Servidor.h"

void verificar_execução() {
    HANDLE h;
    h = CreateMutex(NULL, TRUE, TEXT("instancia"));
    if (h != NULL) {
        switch (GetLastError()) {
        case ERROR_SUCCESS:
            _tprintf(TEXT("\nPrimeira instancia do servidor!\nBEM-VINDO"));
            break;
        case ERROR_ALREADY_EXISTS:
            _tprintf(TEXT("\nUma instancia do servidor já esta a ser corrido, o programa irá fechar"));
            exit(0);
        default:
            break;
        }
    }
}

int isNumberArgv(int * argc, LPTSTR* argv) {
    for (int i = 1; i < *argc; i++)
    {
        for (int j = 0; argv[i][j] != '\0'; j++)
        {
            if (isdigit(argv[i][j]) == 0)
                return 0;
        }
    }
    return 1;
}

void regeditException(DWORD* status, HKEY* chave, TCHAR* nome, TCHAR* caminho, ControlData* cData) {
    TCHAR data[3][TAM];
    if (*status == REG_CREATED_NEW_KEY) {
        _tprintf(TEXT("\nParametros inválidos e regedit vazia \n A terminar...."));
        cData->shutdown = 1;
        Sleep(10000);
        exit(-1);
    }
    else {
        _tprintf(TEXT("\nParametros inválidos obter dados da regedit"));
        for (int i = 0; i < 3; ++i) {
            DWORD size = sizeof(*(data + i));
            if (RegQueryValueEx(chave, nome[i], NULL, NULL, &data[i], &size) != ERROR_SUCCESS) {
                _tprintf(TEXT("\nDados na regedit inválidos ou inexistentes"));
                cData->shutdown = 1;
                Sleep(10000);
                exit(-1);
            }
            for (int j = 0; data[i][j] != '\0'; j++) {
                if (isdigit(data[i][j]) == 0) {
                    _tprintf(TEXT("\nParametros da regedit inválidos"));
                    cData->shutdown = 1;
                    Sleep(10000);
                    exit(-1);
                }
            }
        }
        cData->tam_x = _ttoi(*(data + 0));
        cData->tam_y = _ttoi(*(data + 1));
        cData->tempo_fluir = _ttoi(*(data + 2));
    }
}

void verificar_parametros(int* argc, LPTSTR* argv, ControlData* cData) {
    HKEY chave;
    TCHAR caminho[TAM] = _T("Software\\AULA\\SO2\\TRABALHO\\Game_Info");
    TCHAR data[3][TAM], valor[3][TAM];
    TCHAR nome[][TAM] = {
            TEXT("tam_x"),
            TEXT("tam_y"),
            TEXT("tempo_fluir")
    };
    DWORD status = 0;
    DWORD cSubKeys = 0;
    DWORD cValues;

    if (RegCreateKeyEx(HKEY_CURRENT_USER,
        caminho,
        0,
        NULL,
        REG_OPTION_VOLATILE,
        KEY_ALL_ACCESS,
        NULL,
        &chave,
        &status) == ERROR_SUCCESS) {

        if (status == REG_CREATED_NEW_KEY) {
            _tprintf(TEXT("\nChave criada com sucesso!"));
        }
        else {
            _tprintf(TEXT("\nChave já criada"));
        }
    }
    else {
        _tprintf(TEXT("\nErro a criar ou aceder á chave"));
        cData->shutdown = 1;
        Sleep(10000);
        exit(-1);
    }

    RegQueryInfoKey(
        chave,                    // key handle 
        NULL,                // buffer for class name 
        NULL,           // size of class string 
        NULL,                    // reserved 
        &cSubKeys,               // number of subkeys 
        NULL,            // longest subkey size 
        NULL,            // longest class string 
        &cValues,                // number of values for this key 
        NULL,            // longest value name 
        NULL,         // longest value data 
        NULL,   // security descriptor 
        NULL);       // last write time 


    if (*argc == 4) {
        _tprintf(TEXT("\nParametros da linha de comandos recebidos"));
        if (isNumberArgv(argc, argv)) {
            cData->tam_x = _ttoi(*(argv + 1));
            cData->tam_y = _ttoi(*(argv + 2));
            cData->tempo_fluir = _ttoi(*(argv + 3));
            wsprintf(valor[0], TEXT("%d"), cData->tam_x);
            wsprintf(valor[1], TEXT("%d"), cData->tam_y);
            wsprintf(valor[2], TEXT("%d"), cData->tempo_fluir);

            for (int i = 0; i < 3; ++i) {
                if (RegSetValueEx(chave, nome[i], NULL, REG_SZ, valor[i], TAM) != ERROR_SUCCESS) {
                    _tprintf(TEXT("\nErro ao adicionar chave %s"), nome[i]);
                }
            }
        }
        else {
            if (cValues != 3) {
                _tprintf(TEXT("\nParametros da regedit inválidos"));
                cData->shutdown = 1;
                Sleep(10000);
                exit(-1);
            }
            regeditException(&status, &chave, &nome, &caminho, cData);
        }
    }
    else if (*argc == 1) {

        if (cValues != 3) {
            _tprintf(TEXT("\nParametros da regedit inválidos"));
            cData->shutdown = 1;
            Sleep(10000);
            exit(-1);
        }

        _tprintf(TEXT("\nA buscar parametros á regedit"));
        for (int i = 0; i < 3; ++i) {
            DWORD size = sizeof(data[i]);
            if (RegQueryValueEx(chave, nome[i], NULL, NULL, &data[i], &size) != ERROR_SUCCESS) {
                _tprintf(TEXT("\nDados na regedit inválidos ou inexistentes"));
                cData->shutdown = 1;
                Sleep(10000);
                exit(-1);
            }
            for (int j = 0; data[i][j] != '\0'; j++)
            {
                if (isdigit(data[i][j]) == 0) {
                    _tprintf(TEXT("\nParametros da regedit inválidos"));
                    cData->shutdown = 1;
                    Sleep(10000);
                    exit(-1);
                }
            }
        }
        cData->tam_x = _ttoi(data[0]);
        cData->tam_y = _ttoi(data[1]);
        cData->tempo_fluir = _ttoi(data[2]);
    }
    else {
        regeditException(&status, &chave, &nome, &caminho, cData);
    }
    RegCloseKey(chave);
}

int getRandomNumber(int max, int min) {
    return (rand() % (max - min + 1)) + min;
}

void setStartAndEnd(DADOS_JOGO* dados) {
    if (dados->tam_x == NULL || dados->tam_y == NULL || dados->tam_x == 0 || dados->tam_y == 0) {
        _tprintf(TEXT("Dados do jogo invalidos"));
    }
    else {
        int max_x, max_y, aux = 0;
        max_x = dados->tam_x;
        max_y = dados->tam_y;

        do {
            dados->sp_x = getRandomNumber(max_x, 1);
            dados->sp_y = getRandomNumber(max_y, 1);
            if (dados->sp_x == 1 || dados->sp_x == max_x || dados->sp_y == 1 || dados->sp_y == max_y) {
                aux = 1;
            }
        } while (aux == 0);

        int aux_x = 0, aux_y = 0;
        if (dados->sp_y != dados->sp_x || dados->sp_y != max_y && dados->sp_x != max_x) {
            aux_x = max_x - dados->sp_x;
            aux_y = max_y - dados->sp_y;

            dados->ep_x = aux_x + 1;
            dados->ep_y = aux_y + 1;
        }
        else {
            if (dados->sp_y == 1) {
                dados->ep_x = max_x;
                dados->ep_y = max_y;
            }
            else {
                dados->ep_x = 1;
                dados->ep_y = 1;
            }
        }
    }
}

void decidirDirecaoInicioFim(DADOS_JOGO* dados, int y, int x, enum PATH flag) {
    //DADOS_JOGO* dados = (DADOS_JOGO*)dadospt;
    int tam_y = dados->tam_y - 1, tam_x = dados->tam_x - 1;
    int entrada = 0;
    int saida = 1;
    dados->mapa[y][x].path = flag;
    dados->mapa[y][x].agua = 1;

    if (flag == END) {
        entrada = 1;
        saida = 0;
        dados->mapa[y][x].agua = 0;
    }

    if (y == tam_y) {
        dados->mapa[y][x].cano_pos = 2;
        dados->mapa[y][x].entrada_possiveis[entrada] = NOTHING;
        dados->mapa[y][x].entrada_possiveis[saida] = UP;
        return;
    }
    else if (y == 0) {
        dados->mapa[y][x].cano_pos = 2;
        dados->mapa[y][x].entrada_possiveis[entrada] = NOTHING;
        dados->mapa[y][x].entrada_possiveis[saida] = DOWN;
        return;
    }
    else if (x == tam_x) {
        dados->mapa[y][x].cano_pos = 1;
        dados->mapa[y][x].entrada_possiveis[entrada] = NOTHING;
        dados->mapa[y][x].entrada_possiveis[saida] = LEFT;
        return;
    }
    else if (x == 0) {
        dados->mapa[y][x].cano_pos = 1;
        dados->mapa[y][x].entrada_possiveis[entrada] = NOTHING;
        dados->mapa[y][x].entrada_possiveis[saida] = RIGHT;
        return;
    }
    enum Direction options[] = { UP, DOWN, LEFT, RIGHT };
    dados->mapa[y][x].cano_pos = getRandomNumber(2, 1);

    dados->mapa[y][x].entrada_possiveis[entrada] = NOTHING;
    dados->mapa[y][x].entrada_possiveis[saida] = options[getRandomNumber(3, 0)];
}

void preparar_matriz(DADOS_JOGO* dados_jogo) {

    decidirDirecaoInicioFim(dados_jogo, dados_jogo->sp_y - 1, dados_jogo->sp_x - 1, START);//validações para inicio e fim caso esta encostado nas bordas

    dados_jogo->agua_posY = dados_jogo->sp_y - 1;
    dados_jogo->agua_posX = dados_jogo->sp_x - 1;

    decidirDirecaoInicioFim(dados_jogo, dados_jogo->ep_y - 1, dados_jogo->ep_x - 1, END);
}

void defineEntradas(int* c, enum Direction* ep) {
    switch (*c) {
    case 1:
        ep[0] = LEFT;
        ep[1] = RIGHT;
        return;

    case 2:
        ep[0] = UP;
        ep[1] = DOWN;
        return;
    case 3:
        ep[0] = DOWN;
        ep[1] = RIGHT;
        return;
    case 4:
        ep[0] = DOWN;
        ep[1] = LEFT;
        return;
    case 5:
        ep[0] = UP;
        ep[1] = LEFT;
        return;
    case 6:
        ep[0] = UP;
        ep[1] = RIGHT;
        return;
    }
}

void addCano(TCHAR* op[10], DADOS_JOGO* dados) {
    int y = _ttoi(op[2]) - 1;
    int x = _ttoi(op[3]) - 1;
    dados->mapa[y][x].cano_pos = _ttoi(op[1]);
    dados->mapa[y][x].agua = 0;
    defineEntradas(&(dados->mapa[y][x].cano_pos), &(dados->mapa[y][x].entrada_possiveis));//define as entradas possiveis consoante o cano que for
}

void writeMemory(ControlData* cData) {

    WaitForSingleObject(cData->hWriteSM, INFINITE);
    ReleaseSemaphore(cData->hReadSM, cData->sharedMem->m, NULL);
    
}

void execComando(TCHAR* comando, ControlData* cData) {
    if (!_tcscmp(comando, TEXT("\0"))) {
        return;
    }
    TCHAR* tokens;
    //comando[_tcslen(comando) - 1] = '\0';
    TCHAR** aux = malloc(sizeof(TCHAR*) * 10);
    tokens = _tcstok_s(comando, TEXT(" "), aux);
    TCHAR* op[10];
    int i = 0;
    while (tokens != NULL) {
        op[i] = tokens;
        //_tprintf(TEXT("|%s| \n"), tokens);
        tokens = _tcstok_s(NULL, TEXT(" "), aux);
        i++;
    }
    if (_tcscmp(op[0], TEXT("add")) == 0) {
        int y = 0, x = 0, j = 0;
        y = _ttoi(op[2]);
        x = _ttoi(op[3]);
        //_tprintf(TEXT("|%d||%d||%d||%d| \n"),y, cData->dados->tam_y,x, cData->dados->tam_x);
        if ((y > cData->dados->tam_y || y < 1) || (x > cData->dados->tam_x && x < 1)) {//VERIFICA SE POSICAO É VALIDA
            _tprintf(TEXT("\nPosição Inválida\n"));
            return;
        }
        j = _ttoi(op[1]);
        if (j < 1 || j>6) {//VERIFICA SE CANO É VALIDO
            _tprintf(TEXT("\nCano Inválida"));
            return;
        }
        _tprintf(TEXT("\nComando Reconhecido"));
        addCano(op, cData->dados);
    }
    else if (_tcscmp(op[0], TEXT("quit")) == 0) {
        _tprintf(TEXT("\nComando Reconhecido"));
        _tprintf(TEXT("\nA terminar.."));
        cData->dados->shutdown = 1;
        ReleaseMutex(cData->hMutex);
        ReleaseSemaphore(cData->hReadMS, 1, NULL);
        TerminateThread(cData->hThreads[1], NULL);
    }
    else {
        _tprintf(TEXT("\nComando não Reconhecido"));
        return;

    }
    writeMemory(cData);
}

DWORD WINAPI threadRead(LPVOID a) {
    ControlData* cData = (ControlData*)a;

    while (cData->shutdown == 0) {
        TCHAR comando[TAM];
        WaitForSingleObject(cData->hReadMS, INFINITE);
        WaitForSingleObject(cData->hMutex, INFINITE);
        execComando(cData->sharedMem->comandos[cData->lerPos], cData);
        cData->lerPos++;
        if (cData->lerPos == BUFFER_SIZE) {
            cData->lerPos = 0;
        }
        ReleaseMutex(cData->hMutex);
        ReleaseSemaphore(cData->hWriteMS, 1, NULL);
    }
}

void memoria_partilhada(ControlData* cData, HANDLE* hMapFile) {
    boolean first = FALSE;
    *hMapFile = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,
        FALSE,
        NOMEMP);

    if (*hMapFile == NULL) {
        _tprintf(TEXT("\nVOU CRIAR NOVO FICHEIRO MAPEADO"));
        *hMapFile = CreateFileMapping(
            INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(SharedMem), NOMEMP);
        first = TRUE;
        if (*hMapFile == NULL) {
            _tprintf(TEXT("\nErro no CreateFileMapping."));
            return 1;
        };
    }
    cData->sharedMem = (SharedMem*)MapViewOfFile(*hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (cData->sharedMem == NULL) {
        _tprintf(TEXT("\nErro no MapViewOfFile."));
        return 1;
    }

    if (first) {
        cData->shutdown = 0;
        cData->sharedMem->m = 1;
    }



    cData->hThreads[0] = CreateThread(NULL, 0, threadRead, cData, 0, NULL);

}

BOOL verificaContinuidade(enum Direction d1, enum Direction d2) {
    switch (d1) {
    case UP:
        if (d2 == DOWN) {
            return TRUE;
        }
        else {
            return FALSE;
        }
    case DOWN:
        if (d2 == UP) {
            return TRUE;
        }
        else {
            return FALSE;
        }
    case LEFT:
        if (d2 == RIGHT) {
            return TRUE;
        }
        else {
            return FALSE;
        }
    case RIGHT:
        if (d2 == LEFT) {
            return TRUE;
        }
        else {
            return FALSE;
        }
    case NOTHING:
        return FALSE;
    }
}

DWORD WINAPI aguaFluir(LPVOID a) {
    DADOS_JOGO* dados = (DADOS_JOGO*)a;
    Sleep(dados->tempo_fluir);
    _tprintf(TEXT("\nÁgua começou fluir"));
    dados->code = 1;

    while (dados->shutdown == 0) {
        enum Direction aux_change_order;
        int pos_entrada = -1;

        int y = dados->agua_posY, x = dados->agua_posX;



        switch (dados->mapa[y][x].entrada_possiveis[1]) {
        case UP:
            y--;
            break;
        case DOWN:
            y++;
            break;
        case LEFT:
            x--;
            break;
        case RIGHT:
            x++;
            break;
        }
        for (int i = 0; i < 2; i++)
        {
            //verifica se na pos seguinte tem uma entrada posivel que é contraria a saida pos atual
            //se tiver a 0 essa entrada deixa o array como esta
            //se não estiver modifica o array colocando a entrada na pos[0] e a saida na pos[1] ficilitando na proxima iteração
            if (verificaContinuidade(dados->mapa[dados->agua_posY][dados->agua_posX].entrada_possiveis[1], dados->mapa[y][x].entrada_possiveis[i])) {
                pos_entrada = i;
                dados->agua_posY = y;
                dados->agua_posX = x;
                dados->mapa[y][x].agua = 1;
                if (y == (dados->ep_y - 1) && x == (dados->ep_x - 1)) {
                    _tprintf(TEXT("\nGanhou jogo"));
                    dados->code = 3;
                    dados->shutdown = 1;
                    /*
                    ReleaseMutex(cData->hMutex);
                    ReleaseSemaphore(cData->hReadMS, 1, NULL);*/
                }
                break;
            }
        }
        if (pos_entrada == 1) {
            aux_change_order = dados->mapa[dados->agua_posY][dados->agua_posX].entrada_possiveis[0];
            dados->mapa[dados->agua_posY][dados->agua_posX].entrada_possiveis[0] = dados->mapa[dados->agua_posY][dados->agua_posX].entrada_possiveis[1];
            dados->mapa[dados->agua_posY][dados->agua_posX].entrada_possiveis[1] = aux_change_order;
        }
        else if (pos_entrada == -1) {
            _tprintf(TEXT("\nPerdeu jogo"));
            dados->code = 2;
            dados->shutdown = 1;
        }
        //writeMemory(cData);
        dados->code = 0;
        Sleep(4000);
    }
}

void inicializarEstrutura(ControlData * cData, DADOS_JOGO* dados_jogo) {
    dados_jogo->agua_posX = 0;
    dados_jogo->agua_posY = 0;
    dados_jogo->ep_x = 0;
    dados_jogo->ep_y = 0;
    dados_jogo->sp_x = 0;
    dados_jogo->sp_y = 0;
    dados_jogo->tam_x = cData->tam_x;
    dados_jogo->tam_y = cData->tam_y;
    dados_jogo->tempo_fluir = cData->tempo_fluir;
    dados_jogo->modoAleatorio = FALSE;
    for (int i = 0; i < 20; i++)
    {
        for (int j = 0; j < 20; j++)
        {
            dados_jogo->mapa[i][j].cano_pos = 0;
            dados_jogo->mapa[i][j].entrada_possiveis[0] = NOTHING;
            dados_jogo->mapa[i][j].entrada_possiveis[1] = NOTHING;
            dados_jogo->mapa[i][j].path = NORMAL;
            dados_jogo->mapa[i][j].agua = 0;
        }
    }
}

void iniciarSemaforoMutex(ControlData* cData) {
    cData->hMutex = CreateMutex(NULL, FALSE, MUTEX_NAME);
    if (cData->hMutex == NULL) {
        _tprintf(TEXT("\nErro no CreateMutex Servidor."));
        return 1;
    }


    //MONITOR - SERVIDOR

    cData->hWriteMS = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, MS_WRITE_NAME);

    cData->hReadMS = CreateSemaphore(NULL, 0, BUFFER_SIZE, MS_READ_NAME);

    if (cData->hWriteMS == NULL || cData->hReadMS == NULL) {
        _tprintf(TEXT("\nErro no CreateSemaphore MS."));
        return 1;
    }

    //SERVIDOR-MONITOR

    cData->hWriteSM = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, SM_WRITE_NAME);

    cData->hReadSM = CreateSemaphore(NULL, 0, BUFFER_SIZE, SM_READ_NAME);

    if (cData->hWriteSM == NULL || cData->hReadSM == NULL) {
        _tprintf(TEXT("\nErro no CreateSemaphore SM."));
        return 1;
    }
}


/*
void threadConsola(ThreadDados* d) {
    TCHAR nome[256];
    DWORD n;
    int i;
    BOOL ret;
    ThreadDados* dados = (ThreadDados*)d;

    do {
        /*
        _tprintf(TEXT("[ESCRITOR] Frase: "));
        _fgetts(buf, 256, stdin);
        buf[_tcslen(buf) - 1] = '\0';
        for (i = 0; i < MAX_CLI; i++) {
            WaitForSingleObject(dados->hMutex, INFINITE);
            if (dados->hPipes[i].activo) {
                /*
                if (!WriteFile(dados->hPipes[i].hInstancia, buf, _tcslen(buf) * sizeof(TCHAR), &n, NULL))
                    _tprintf(TEXT("[ERRO] Escrever no pipe! (WriteFile)\n"));
                else {
                    _tprintf(TEXT("[ESCRITOR] Enviei %d bytes ao leitor [%d]... (WriteFile)\n"), n, i);
                    
                ret = ReadFile(dados->hPipes[i].hInstancia, nome, sizeof(nome), &n, NULL);
                _tprintf(TEXT("[ESCRITOR] Recebi %d bytes: '%s'... (ReadFile)\n"), n, nome);
                //}
            }
            ReleaseMutex(dados->hMutex);
        }
    } while (_tcscmp(nome, TEXT("FIM")));
    for (i = 0; i < MAX_CLI; i++)
        SetEvent(dados->hEvents[i]);
    return 0;
}*/

void threadCliente(LPVOID* d) {
    _tprintf(TEXT("ENMTREI"));
    ControlData* cData = (ControlData*)d;
    DWORD n;
    int i;
    BOOL ret;

    cData->hThreads[0] = NULL;
    cData->hThreads[1] = NULL;

    
    WaitForSingleObject(cData->hMutex, INFINITE);
    CLIENTE* cliente = &cData->sharedMem->clientes[cData->c - 1];
    cliente->numero = cData->c-1;
    
    ret = ReadFile(cData->pipe[cliente->numero]->hInstancia, cliente->nome, sizeof(cliente->nome), NULL, NULL);
    _tprintf(TEXT("Nome do cliente %d: %s\n"), cliente->numero+1, cliente->nome);
    ReleaseMutex(cData->hMutex);
    
    inicializarEstrutura(cData, &cliente->dados_jogo);//inicializa toda a estrutura de dados para não ocorrer erros a partilhar

    setStartAndEnd(&cliente->dados_jogo);//define inicio e fim do jogo

    preparar_matriz(&cliente->dados_jogo);//prepara toda a matriz em função do inicio e do fim

    writeMemory(cData);//enviar a informação para o monitor

    cData->hThreads[1] = CreateThread(NULL, 0, aguaFluir, &cliente->dados_jogo, 0, NULL); // iniciar thread que flui a agua

    WaitForMultipleObjects(2, cData->hThreads, TRUE, INFINITE);//esperar fim das threads

    CloseHandle(cData->hThreads[0]);
    CloseHandle(cData->hThreads[1]);

    Sleep(2000);
    ResetEvent(cData->pipe[cliente->numero]->overlap.hEvent);
}


void esperarClientes(ControlData * cData) {
    TCHAR nome[256];
    PIPES dados;
    HANDLE hPipe, hThread[MAX_CLI], hEventTemp;
    int i, numClientes = 0;
    DWORD offset, nBytes;

    for (int p = 0;  p < MAX_CLI;  p++)
    {
        hThread[p] = NULL;
    }

    for (i = 0; i < MAX_CLI; i++) {
        //true para o evento estar sinalizado
        hEventTemp = CreateEvent(NULL, TRUE, FALSE, NULL);
        if (hEventTemp == NULL) {
            _tprintf(TEXT("\n[ERRO] Criar Evento! (CreateEvent)"));
            exit(-1);
        }

        hPipe = CreateNamedPipe(PIPE_NAME,
            PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
            PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE,
            MAX_CLI, sizeof(CLIENTE), sizeof(CLIENTE), 1000, NULL);
        
        if (hPipe == INVALID_HANDLE_VALUE) {
            _tprintf(TEXT("\n[ERRO] Criar Named Pipe! (CreateNamedPipe)"));
            exit(-1);
        }
        //ZeroMemory(destino, tamanho)
        //conforme a documentação meter a memoria a estrutura overlap a zero
        ZeroMemory(&dados.hPipes[i].overlap, sizeof(dados.hPipes[i].overlap));
        dados.hPipes[i].hInstancia = hPipe;		//handle do pipe
        dados.hPipes[i].overlap.hEvent = hEventTemp;	//handle do evento (dentro da estrutura)
        dados.hEvents[i] = hEventTemp;		//handle do evento
        dados.hPipes[i].activo = FALSE;		//flag de sinalizaçao

        //o segundo parametro é o ponteiro para a estrutura de dados overlap
        if (ConnectNamedPipe(hPipe, &dados.hPipes[i].overlap)) {
            _tprintf(TEXT("\n[ERRO] Ligar ao Cliente! (ConnectNamedPipe)"));
            exit(-1);
        }
    }
   


    while (numClientes < MAX_CLI) {
        _tprintf(TEXT("\nÀ espera de clientes..."));

        //3º parametro FALSE, the function returns when the state of any one of the objects is set to signaled

        offset = WaitForMultipleObjects(MAX_CLI, dados.hEvents, FALSE, INFINITE);
        i = offset - WAIT_OBJECT_0;

        if (i >= 0 && i < MAX_CLI) {
            
            _tprintf(TEXT("\nCliente %d entrou..."), i+1);
            //identificador para o ficheiro named pipe 
            //ponteiro para a estrutura overlap
            //ponteiro para uma variavel que recebe o numero de bytes
            //true -> se estiver a true e o membro interno da estrutura lpOverlaped for STATUS_PENDING, a funcao nao retornara
            //false -> e a operaçao ainda estiver pendente, a funcao retornara FALSE e a funcao GetLastError retornara ERROR_I

            if (GetOverlappedResult(dados.hPipes[i].hInstancia,
                &dados.hPipes[i].overlap, &nBytes, FALSE)) {
                ResetEvent(dados.hEvents[i]);

                WaitForSingleObject(cData->hMutex, INFINITE);
                cData->c++;
                dados.hPipes[i].activo = TRUE;
                cData->pipe[i] = &dados.hPipes[i];
                hThread[i] = CreateThread(NULL, 0, threadCliente, cData, 0, NULL);
                if (hThread == NULL) {
                    _tprintf(TEXT("\n[ERRO] Criar Thread! (CreateThread)"));
                    exit(-1);
                }
                
                ReleaseMutex(cData->hMutex);
                
            }
            numClientes++;
        }
    }
    WaitForMultipleObjects(MAX_CLI, hThread, TRUE, INFINITE);

    for (i = 0; i < MAX_CLI; i++) {
        _tprintf(TEXT("\nDesligar o pipe (DisconnectNamedPipe)"));

        if (!DisconnectNamedPipe(dados.hPipes[i].hInstancia)) {
            _tprintf(TEXT("\n[ERRO] Desligar o pipe! (DisconnectNamedPipe)"));
            exit(-1);
        }
        CloseHandle(dados.hPipes[i].hInstancia);
    }
}

int _tmain(int argc, LPTSTR argv[]) {
    DADOS_JOGO* dados_jogo;
    ControlData cData;
    HANDLE hMapFile;

#ifdef UNICODE
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
    _setmode(_fileno(stderr), _O_WTEXT);
#endif
    hMapFile = NULL;
    cData.escreverPos = 0;
    cData.lerPos = 0;
    cData.c = 0;
    srand(time(0));

    verificar_execução(); // verifica se ja existe servidor 
    iniciarSemaforoMutex(&cData);
    memoria_partilhada(&cData, &hMapFile);//inicia a memoria partilhada

    verificar_parametros(&argc, argv, &cData);// verifica os parametros da linha de comandos e da regedit

    //dados_jogo = &cData.sharedMem->Dados_Partilhados;
    //cData.dados = dados_jogo;

    // verificar clientes aqui
    esperarClientes(&cData);


    //fechar tudo
    UnmapViewOfFile(cData.sharedMem);
    CloseHandle(hMapFile);
    CloseHandle(cData.hMutex);
    CloseHandle(cData.hWriteMS);
    CloseHandle(cData.hReadMS);
    CloseHandle(cData.hWriteSM);
    CloseHandle(cData.hReadSM);

    return 0;
}

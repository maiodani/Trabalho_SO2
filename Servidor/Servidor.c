#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "Servidor.h"

void verificar_execu��o() {
    HANDLE h;
    h = CreateMutex(NULL, TRUE, TEXT("instancia"));
    if (h != NULL) {
        switch (GetLastError()) {
        case ERROR_SUCCESS:
            _tprintf(TEXT("\nPrimeira instancia do servidor!\nBEM-VINDO"));
            break;
        case ERROR_ALREADY_EXISTS:
            _tprintf(TEXT("\nUma instancia do servidor j� esta a ser corrido, o programa ir� fechar"));
            exit(0);
        default:
            break;
        }
    }
}

int isNumberArgv(int argc, LPTSTR* argv) {
    for (int i = 1; i < argc; i++)
    {
        for (int j = 0; argv[i][j] != '\0'; j++)
        {
            if (isdigit(argv[i][j]) == 0)
                return 0;
        }
    }
    return 1;
}

void regeditException(DWORD* status, HKEY* chave, TCHAR* nome, TCHAR* caminho, DADOS_JOGO* dados) {
    TCHAR data[3][TAM];
    if (*status == REG_CREATED_NEW_KEY) {
        _tprintf(TEXT("\nParametros inv�lidos e regedit vazia \n A terminar...."));
        dados->shutdown = 1;
        Sleep(10000);
        exit(-1);
    }
    else {
        _tprintf(TEXT("\nParametros inv�lidos obter dados da regedit"));
        for (int i = 0; i < 3; ++i) {
            DWORD size = sizeof(*(data + i));
            if (RegQueryValueEx(chave, nome[i], NULL, NULL, &data[i], &size) != ERROR_SUCCESS) {
                _tprintf(TEXT("\nDados na regedit inv�lidos ou inexistentes"));
                dados->shutdown = 1;
                Sleep(10000);
                exit(-1);
            }
            for (int j = 0; data[i][j] != '\0'; j++) {
                if (isdigit(data[i][j]) == 0) {
                    _tprintf(TEXT("\nParametros da regedit inv�lidos"));
                    dados->shutdown = 1;
                    Sleep(10000);
                    exit(-1);
                }
            }
        }
        dados->tam_x = _ttoi(*(data + 0));
        dados->tam_y = _ttoi(*(data + 1));
        dados->tempo_fluir = _ttoi(*(data + 2));
    }
}

void verificar_parametros(int* argc, LPTSTR* argv, DADOS_JOGO* dados) {
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
            _tprintf(TEXT("\nChave j� criada"));
        }
    }
    else {
        _tprintf(TEXT("\nErro a criar ou aceder � chave"));
        dados->shutdown = 1;
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
            dados->tam_x = _ttoi(*(argv + 1));
            dados->tam_y = _ttoi(*(argv + 2));
            dados->tempo_fluir = _ttoi(*(argv + 3));
            wsprintf(valor[0], TEXT("%d"), dados->tam_x);
            wsprintf(valor[1], TEXT("%d"), dados->tam_y);
            wsprintf(valor[2], TEXT("%d"), dados->tempo_fluir);

            for (int i = 0; i < 3; ++i) {
                if (RegSetValueEx(chave, nome[i], NULL, REG_SZ, valor[i], TAM) != ERROR_SUCCESS) {
                    _tprintf(TEXT("\nErro ao adicionar chave %s"), nome[i]);
                }
            }
        }
        else {
            if (cValues != 3) {
                _tprintf(TEXT("\nParametros da regedit inv�lidos"));
                dados->shutdown = 1;
                Sleep(10000);
                exit(-1);
            }
            regeditException(&status, &chave, &nome, &caminho, dados);
        }
    }
    else if (*argc == 1) {

        if (cValues != 3) {
            _tprintf(TEXT("\nParametros da regedit inv�lidos"));
            dados->shutdown = 1;
            Sleep(10000);
            exit(-1);
        }

        _tprintf(TEXT("\nA buscar parametros � regedit"));
        for (int i = 0; i < 3; ++i) {
            DWORD size = sizeof(data[i]);
            if (RegQueryValueEx(chave, nome[i], NULL, NULL, &data[i], &size) != ERROR_SUCCESS) {
                _tprintf(TEXT("\nDados na regedit inv�lidos ou inexistentes"));
                dados->shutdown = 1;
                Sleep(10000);
                exit(-1);
            }
            for (int j = 0; data[i][j] != '\0'; j++)
            {
                if (isdigit(data[i][j]) == 0) {
                    _tprintf(TEXT("\nParametros da regedit inv�lidos"));
                    dados->shutdown = 1;
                    Sleep(10000);
                    exit(-1);
                }
            }
        }
        dados->tam_x = _ttoi(data[0]);
        dados->tam_y = _ttoi(data[1]);
        dados->tempo_fluir = _ttoi(data[2]);
    }
    else {
        regeditException(&status, &chave, &nome, &caminho, dados);
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

    decidirDirecaoInicioFim(dados_jogo, dados_jogo->sp_y - 1, dados_jogo->sp_x - 1, START);//valida��es para inicio e fim caso esta encostado nas bordas

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
        if ((y > cData->dados->tam_y || y < 1) || (x > cData->dados->tam_x && x < 1)) {//VERIFICA SE POSICAO � VALIDA
            _tprintf(TEXT("\nPosi��o Inv�lida\n"));
            return;
        }
        j = _ttoi(op[1]);
        if (j < 1 || j>6) {//VERIFICA SE CANO � VALIDO
            _tprintf(TEXT("\nCano Inv�lida"));
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
        _tprintf(TEXT("\nComando n�o Reconhecido"));
        return;

    }
    writeMemory(cData);
}

DWORD WINAPI threadRead(LPVOID a) {
    ControlData* cData = (ControlData*)a;

    while (cData->sharedMem->Dados_Partilhados.shutdown == 0) {
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
        cData->sharedMem->Dados_Partilhados.shutdown = 0;
        cData->sharedMem->m = 0;
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
    ControlData* cData = (ControlData*)a;
    Sleep(cData->dados->tempo_fluir);
    _tprintf(TEXT("\n�gua come�ou fluir"));
    cData->dados->code = 1;

    while (cData->dados->shutdown == 0) {
        enum Direction aux_change_order;
        int pos_entrada = -1;

        int y = cData->dados->agua_posY, x = cData->dados->agua_posX;



        switch (cData->dados->mapa[y][x].entrada_possiveis[1]) {
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
            //verifica se na pos seguinte tem uma entrada posivel que � contraria a saida pos atual
            //se tiver a 0 essa entrada deixa o array como esta
            //se n�o estiver modifica o array colocando a entrada na pos[0] e a saida na pos[1] ficilitando na proxima itera��o
            if (verificaContinuidade(cData->dados->mapa[cData->dados->agua_posY][cData->dados->agua_posX].entrada_possiveis[1], cData->dados->mapa[y][x].entrada_possiveis[i])) {
                pos_entrada = i;
                cData->dados->agua_posY = y;
                cData->dados->agua_posX = x;
                cData->dados->mapa[y][x].agua = 1;
                if (y == (cData->dados->ep_y - 1) && x == (cData->dados->ep_x - 1)) {
                    _tprintf(TEXT("\nGanhou jogo"));
                    cData->dados->code = 3;
                    cData->dados->shutdown = 1;
                    ReleaseMutex(cData->hMutex);
                    ReleaseSemaphore(cData->hReadMS, 1, NULL);
                }
                break;
            }
        }
        if (pos_entrada == 1) {
            aux_change_order = cData->dados->mapa[cData->dados->agua_posY][cData->dados->agua_posX].entrada_possiveis[0];
            cData->dados->mapa[cData->dados->agua_posY][cData->dados->agua_posX].entrada_possiveis[0] = cData->dados->mapa[cData->dados->agua_posY][cData->dados->agua_posX].entrada_possiveis[1];
            cData->dados->mapa[cData->dados->agua_posY][cData->dados->agua_posX].entrada_possiveis[1] = aux_change_order;
        }
        else if (pos_entrada == -1) {
            _tprintf(TEXT("\nPerdeu jogo"));
            cData->dados->code = 2;
            cData->dados->shutdown = 1;
        }
        writeMemory(cData);
        cData->dados->code = 0;
        Sleep(4000);
    }
}

void inicializarEstrutura(DADOS_JOGO* dados_jogo) {
    dados_jogo->agua_posX = 0;
    dados_jogo->agua_posY = 0;
    dados_jogo->ep_x = 0;
    dados_jogo->ep_y = 0;
    dados_jogo->sp_x = 0;
    dados_jogo->sp_y = 0;
    dados_jogo->tam_x = 0;
    dados_jogo->tam_y = 0;
    dados_jogo->tempo_fluir = 0;
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

int _tmain(int argc, LPTSTR argv[]) {
    DADOS_JOGO* dados_jogo;
    ControlData cData;
    HANDLE hMapFile;

#ifdef UNICODE
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
    _setmode(_fileno(stderr), _O_WTEXT);
#endif
    cData.hThreads[0] = NULL;
    cData.hThreads[1] = NULL;
    hMapFile = NULL;
    cData.escreverPos = 0;
    cData.lerPos = 0;
    srand(time(0));

    verificar_execu��o(); // verifica se ja existe servidor 
    iniciarSemaforoMutex(&cData);
    memoria_partilhada(&cData, &hMapFile);//inicia a memoria partilhada

    dados_jogo = &cData.sharedMem->Dados_Partilhados;
    cData.dados = dados_jogo;

    inicializarEstrutura(dados_jogo);//inicializa toda a estrutura de dados para n�o ocorrer erros a partilhar

    verificar_parametros(&argc, argv, dados_jogo);// verifica os parametros da linha de comandos e da regedit

    setStartAndEnd(dados_jogo);//define inicio e fim do jogo

    preparar_matriz(dados_jogo);//prepara toda a matriz em fun��o do inicio e do fim

    writeMemory(&cData);//enviar a informa��o para o monitor

    cData.hThreads[1] = CreateThread(NULL, 0, aguaFluir, &cData, 0, NULL); // iniciar thread que flui a agua

    WaitForMultipleObjects(2, &cData.hThreads, TRUE, INFINITE);//esperar fim das threads

    //fechar tudo
    CloseHandle(cData.hThreads[0]);
    CloseHandle(cData.hThreads[1]);
    UnmapViewOfFile(cData.sharedMem);
    CloseHandle(hMapFile);
    CloseHandle(cData.hMutex);
    CloseHandle(cData.hWriteMS);
    CloseHandle(cData.hReadMS);
    CloseHandle(cData.hWriteSM);
    CloseHandle(cData.hReadSM);

    return 0;
}
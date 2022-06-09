#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>

#include "Monitor.h"

void printBoard(DADOS_JOGO* jogo) {
    TCHAR jogadas[][3] = { _T(" "), _T("━"), _T("┃"), _T("┏"), _T("┓"), _T("┛"), _T("┗") };
    switch (jogo->code)
    {
    case 1:_tprintf(TEXT("\nAgua começou a fluir"));
        break;
    case 2:_tprintf(TEXT("\nPerdeu o jogo"));
        break;
    case 3:_tprintf(TEXT("\nGanhou o jogo"));
        break;
    default:
        break;
    }
    _tprintf(TEXT("\n"));
    _tprintf(TEXT("\nMAPA: "));
    _tprintf(TEXT("\n"));
    _tprintf(TEXT(" x "));
    for (int k = 0; k < jogo->tam_x; k++) {
        _tprintf(TEXT("%d "), k + 1);
    }
    _tprintf(TEXT("\n"));
    _tprintf(TEXT("y"));
    for (int i = 0; i < jogo->tam_y; i++) {
        _tprintf(TEXT("\n"));
        _tprintf(TEXT("%d  "), i + 1);
        for (int j = 0; j < jogo->tam_x; j++) {
            if (jogo->mapa[i][j].agua == 1) {
                _tprintf(TEXT("\033[0;36m%s \033[0m"), jogadas[jogo->mapa[i][j].cano_pos]);
            }
            else {
                _tprintf(TEXT("%s "), jogadas[jogo->mapa[i][j].cano_pos]);
            }
        }

    }
    _tprintf(TEXT("\nSTARTING POINT -> Y:%d X:%d \nENDING POINT -> Y:%d X:%d \n"), jogo->sp_y, jogo->sp_x, jogo->ep_y, jogo->ep_x);
    _tprintf(TEXT("AGUA ESTA NO PONTO -> Y:%d X:%d\n"), jogo->agua_posY + 1, jogo->agua_posX + 1);

    _tprintf(TEXT("1: ━ , 2: ┃ , 3: ┏ , 4: ┓ , 5: ┛ , 6: ┗ \n"));

}

void writeMemory(ControlData* cData, TCHAR* comando) {

    TCHAR dado[TAM];
    wcscpy_s(dado, TAM, comando);

    dado[_tcslen(dado) - 1] = '\0';
    WaitForSingleObject(cData->hWriteMS, INFINITE);
    WaitForSingleObject(cData->hMutex, INFINITE);

    CopyMemory(&cData->sharedMem->comandos[cData->escreverPos], dado, TAM);
    cData->escreverPos++;
    if (cData->escreverPos == BUFFER_SIZE) {
        cData->escreverPos = 0;
    }

    ReleaseMutex(cData->hMutex);
    ReleaseSemaphore(cData->hReadMS, 1, NULL);

    ZeroMemory(dado, sizeof(dado));
}

DWORD WINAPI threadRead(LPVOID a) {
    ControlData* cData = (ControlData*)a;
    while (cData->dados->shutdown == 0) {
        WaitForSingleObject(cData->hReadSM, INFINITE);
        WaitForSingleObject(cData->hMutex, INFINITE);

        printBoard(&cData->sharedMem->clientes[0].dados_jogo);//imprimir a board sempre que receber nova informação

        ReleaseMutex(cData->hMutex);
        ReleaseSemaphore(cData->hWriteSM, 1, NULL);

    }
}



void memoria_partilhada(ControlData* cData, HANDLE* hThread, HANDLE* hMapFile) {
    boolean first = FALSE;

    *hMapFile = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,
        FALSE,
        NOMEMP);

    if (*hMapFile == NULL) {
        _tprintf(TEXT("VOU CRIAR NOVO FICHEIRO MAPEADO"));
        *hMapFile = CreateFileMapping(
            INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(SharedMem), NOMEMP);
        first = TRUE;
        if (*hMapFile == NULL) {
            _tprintf(TEXT("Erro no CreateFileMapping.\n"));
            return 1;
        };
    }
    cData->sharedMem = (SharedMem*)MapViewOfFile((*hMapFile), FILE_MAP_ALL_ACCESS, 0, 0, 0);

    if (cData->sharedMem == NULL) {
        _tprintf(TEXT("Erro no MapViewOfFile.\n"));
        return 1;
    }


    if (first) {
        cData->shutdown = 0;
        cData->sharedMem->m = 1;
    }
    else {
        cData->sharedMem->m++;
    }


    *hThread = CreateThread(NULL, 0, threadRead, cData, 0, NULL);
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

void lerComandos(ControlData* cData) {
    TCHAR buf[TAM];

    do {
        if (cData->dados->shutdown == 1) {
            return;
        }
        ZeroMemory(buf, sizeof(buf));
        _fgetts(buf, TAM, stdin);
        writeMemory(cData, buf);//envia o comando para o servidor
    } while (_tcscmp(buf, TEXT("quit\n")));
    Sleep(1000);
    ReleaseMutex(cData->hMutex);
    ReleaseSemaphore(cData->hReadSM, 1, NULL);
}

int _tmain(int argc, LPTSTR argv[]) {
    DADOS_JOGO* dados_jogo;
    ControlData cData;
    HANDLE hThread, hMapFile;
    TCHAR comando[TAM];

#ifdef UNICODE
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
    _setmode(_fileno(stderr), _O_WTEXT);
#endif
    Sleep(10000);
    hThread = NULL;
    hMapFile = NULL;
    cData.escreverPos = 0;
    cData.lerPos = 0;
    iniciarSemaforoMutex(&cData);
    memoria_partilhada(&cData, &hThread, &hMapFile);//iniciar memoria partilhada

    dados_jogo = &cData.sharedMem->clientes[0].dados_jogo;
    cData.dados = dados_jogo;
    lerComandos(&cData);//ler comandos do utilizador

    WaitForSingleObject(hThread, INFINITE);//esperar thread terminar

    //fechar tudo
    CloseHandle(hThread);
    UnmapViewOfFile(cData.sharedMem);
    CloseHandle(hMapFile);
    CloseHandle(cData.hMutex);
    CloseHandle(cData.hWriteMS);
    CloseHandle(cData.hReadMS);
    CloseHandle(cData.hWriteSM);
    CloseHandle(cData.hReadSM);
    return 0;
}
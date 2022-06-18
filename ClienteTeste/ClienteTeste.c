//devido a não saber onde colocar o codigo na versão grafica criei este cliente teste para começar a preparar o servidor
#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "ClienteTeste.h"

//PARA O EU DO FUTURO ISTO N ESTA TOTALMENTE CERTO MAS ESTA UM POUCO :)


void printBoard(CLIENTE* cliente) {
	TCHAR jogadas[][3] = { _T(" "), _T("━"), _T("┃"), _T("┏"), _T("┓"), _T("┛"), _T("┗"), _T("□") };

	
	DADOS_JOGO* jogo = &cliente->dados_jogo;
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
				_tprintf(TEXT("a%s"), jogadas[jogo->mapa[i][j].cano_pos]);
			}
			else {
				_tprintf(TEXT("%s "), jogadas[jogo->mapa[i][j].cano_pos]);
			}
		}

	}
	_tprintf(TEXT("\nSTARTING POINT -> Y:%d X:%d \nENDING POINT -> Y:%d X:%d \n"), jogo->sp_y, jogo->sp_x, jogo->ep_y, jogo->ep_x);
	_tprintf(TEXT("AGUA ESTA NO PONTO -> Y:%d X:%d\n"), jogo->agua_posY + 1, jogo->agua_posX + 1);

	for (int i = 0; i < 6; i++)
	{
		_tprintf(TEXT("%d: %s, "), i, jogadas[jogo->jogadas[i]]);
	}
	_tprintf(TEXT("\n"));
		
	
}

DWORD WINAPI readPipe(LPVOID a) {
	ControlData* cData = (ControlData*)a;
	HANDLE hpipe;
	TCHAR pipe_name[TAM];

	wsprintf(pipe_name, TEXT("\\\\.\\pipe\\CLIENTE_%d_READ"), cData->cliente->ID);

	if (!WaitNamedPipe(pipe_name, NMPWAIT_WAIT_FOREVER)) {
		_tprintf(TEXT("[ERRO] Ligar ao  pipe '%s' ... (WaitNamedPipe)\n"), PIPE_NAME);
		exit(-1);
	}

	_tprintf(TEXT("[CLIENTE] Ligação ao pipe do escritor ... (CreateFile)\n"));

	hpipe = CreateFile(pipe_name, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hpipe == NULL) {
		_tprintf(TEXT("[ERRO] Ligar ao  pipe '%s'! (CreateFile)\n"), pipe_name);
		exit(-1);
	}

	_tprintf(TEXT("[CLIENTE] Liguei-me...\n"));
	while (1) {
		ReadFile(hpipe, &cData->cliente->dados_jogo, sizeof(cData->cliente->dados_jogo), NULL, NULL);
		_tprintf(TEXT("RECEBI\n"));
		printBoard(cData->cliente);
	}
	CloseHandle(hpipe);
}

DWORD WINAPI writePipe(LPVOID a) {
	ControlData* cData = (ControlData*)a;
	TCHAR buf[TAM];
	HANDLE hpipe;
	TCHAR pipe_name[TAM];
	wsprintf(pipe_name, TEXT("\\\\.\\pipe\\CLIENTE_%d_WRITE"), cData->cliente->ID);

	if (!WaitNamedPipe(pipe_name, NMPWAIT_WAIT_FOREVER)) {
		_tprintf(TEXT("[ERRO] Ligar ao  pipe '%s' ... (WaitNamedPipe)\n"), PIPE_NAME);
		exit(-1);
	}

	_tprintf(TEXT("[CLIENTE] Ligação ao pipe do escritor ... (CreateFile)\n"));

	hpipe = CreateFile(pipe_name, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hpipe == NULL) {
		_tprintf(TEXT("[ERRO] Ligar ao  pipe '%s'! (CreateFile)\n"), pipe_name);
		exit(-1);
	}

	do {
		if (cData->shutdown == 1) {
			return;
		}
		_tprintf(TEXT("\nInsira:"));
		_fgetts(buf, TAM, stdin);
		buf[_tcslen(buf) - 1] = '\0';
		if (!WriteFile(hpipe, buf, (DWORD)_tcslen(buf) * sizeof(TCHAR), NULL, NULL)) {
			_tprintf(TEXT("[ERRO] Escrever no pipe! (WriteFile)\n"));
		}
	
	} while (_tcscmp(buf, TEXT("quit")));

	CloseHandle(hpipe);
}

int _tmain(int argc, LPTSTR argv[]) {
	ControlData cData;
	CLIENTE cliente;
	TCHAR buf[256];
	HANDLE hpipe;
	unsigned int i = 0;
	BOOL ret;
	DWORD n;
	TCHAR nome[TAM];

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif
    cData.cliente = &cliente;
	cData.shutdown = 0;

	
	_tprintf(TEXT("[LEITOR] Esperar pelo pipe '%s' ... (WaitNamedPipe)\n"), PIPE_NAME);

	if (!WaitNamedPipe(PIPE_NAME, NMPWAIT_WAIT_FOREVER)) {
		_tprintf(TEXT("[ERRO] Ligar ao  pipe '%s' ... (WaitNamedPipe)\n"), PIPE_NAME);
		exit(-1);
	}

	_tprintf(TEXT("[LEITOR] Ligação ao pipe do escritor ... (CreateFile)\n"));

	hpipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hpipe == NULL) {
		_tprintf(TEXT("[ERRO] Ligar ao  pipe '%s'! (CreateFile)\n"), PIPE_NAME);
		exit(-1);
	}

	_tprintf(TEXT("[LEITOR] Liguei-me...\n"));

	ReadFile(hpipe, &cliente, sizeof(cliente), NULL, NULL);
	//ReadFile(hpipe, &cliente, sizeof(cliente), NULL, NULL);
	/*
	ret = ReadFile(hpipe, buf, sizeof(buf) - sizeof(TCHAR), &n, NULL);
	buf[n / sizeof(TCHAR)] = '\0';

	if (!ret || !n) {
		_tprintf(TEXT("[LEITOR] %d %d... (ReadFile)\n"), ret, n);
		break;
	}

	_tprintf(TEXT("[LEITOR] Recebi %d bytes: '%s' ... (ReadFile)\n"), n, buf);
	*/
	_tprintf(TEXT("[CLIENTE] nome: "));
	_fgetts(nome, TAM, stdin);
	nome[_tcslen(nome) - 1] = '\0';
	wcscpy_s(cliente.nome, TAM, nome);
	cliente.ID = GetCurrentProcessId();
	if (!WriteFile(hpipe, &cliente, sizeof(cliente), &n, NULL))
		_tprintf(TEXT("[ERRO] Escrever no pipe! (WriteFile)\n"));
	else
		_tprintf(TEXT("[LEITOR] Enviei %d bytes ao escritor [%d]... (writeFile)\n"), n, i);

	cData.pipe = &hpipe;


	HANDLE threadRead = CreateThread(NULL, 0, readPipe, &cData, 0, NULL); // iniciar thread que flui a agua

	HANDLE threadWrite = CreateThread(NULL, 0, writePipe, &cData, 0, NULL); // iniciar thread que flui a agua
	
	WaitForSingleObject(threadWrite, INFINITE);
	WaitForSingleObject(threadRead, INFINITE);

	CloseHandle(hpipe);
	CloseHandle(threadWrite);
	CloseHandle(threadRead);

	return 0;
}
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "Cliente.h"


/* ===================================================== */
/* Programa base (esqueleto) para aplicações Windows     */
/* ===================================================== */
// Cria uma janela de nome "Janela Principal" e pinta fundo de branco
// Modelo para programas Windows:
//  Composto por 2 funções: 
//	WinMain()     = Ponto de entrada dos programas windows
//			1) Define, cria e mostra a janela
//			2) Loop de recepção de mensagens provenientes do Windows
//     TrataEventos()= Processamentos da janela (pode ter outro nome)
//			1) É chamada pelo Windows (callback) 
//			2) Executa código em função da mensagem recebida

LRESULT CALLBACK TrataEventos(HWND, UINT, WPARAM, LPARAM);

// Nome da classe da janela (para programas de uma só janela, normalmente este nome é 
// igual ao do próprio programa) "szprogName" é usado mais abaixo na definição das 
// propriedades do objecto janela
TCHAR szProgName[] = TEXT("Base");

// ============================================================================
// FUNÇÃO DE INÍCIO DO PROGRAMA: WinMain()
// ============================================================================
// Em Windows, o programa começa sempre a sua execução na função WinMain()que desempenha
// o papel da função main() do C em modo consola WINAPI indica o "tipo da função" (WINAPI
// para todas as declaradas nos headers do Windows e CALLBACK para as funções de
// processamento da janela)
// Parâmetros:
//   hInst: Gerado pelo Windows, é o handle (número) da instância deste programa 
//   hPrevInst: Gerado pelo Windows, é sempre NULL para o NT (era usado no Windows 3.1)
//   lpCmdLine: Gerado pelo Windows, é um ponteiro para uma string terminada por 0
//              destinada a conter parâmetros para o programa 
//   nCmdShow:  Parâmetro que especifica o modo de exibição da janela (usado em  
//        	   ShowWindow()

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
	HWND hWnd;		// hWnd é o handler da janela, gerado mais abaixo por CreateWindow()
	MSG lpMsg;		// MSG é uma estrutura definida no Windows para as mensagens
	WNDCLASSEX wcApp;	// WNDCLASSEX é uma estrutura cujos membros servem para 
			  // definir as características da classe da janela

	// ============================================================================
	// 1. Definição das características da janela "wcApp" 
	//    (Valores dos elementos da estrutura "wcApp" do tipo WNDCLASSEX)
	// ============================================================================
	wcApp.cbSize = sizeof(WNDCLASSEX);      // Tamanho da estrutura WNDCLASSEX
	wcApp.hInstance = hInst;		         // Instância da janela actualmente exibida 
								   // ("hInst" é parâmetro de WinMain e vem 
										 // inicializada daí)
	wcApp.lpszClassName = szProgName;       // Nome da janela (neste caso = nome do programa)
	wcApp.lpfnWndProc = TrataEventos;       // Endereço da função de processamento da janela
											// ("TrataEventos" foi declarada no início e
											// encontra-se mais abaixo)
	wcApp.style = CS_HREDRAW | CS_VREDRAW;  // Estilo da janela: Fazer o redraw se for
											// modificada horizontal ou verticalmente
	wcApp.hIcon = LoadIcon(NULL, IDI_ASTERISK);   // "hIcon" = handler do ícon normal
										   // "NULL" = Icon definido no Windows
										   // "IDI_AP..." Ícone "aplicação"
	wcApp.hIconSm = LoadIcon(NULL, IDI_INFORMATION); // "hIconSm" = handler do ícon pequeno
										   // "NULL" = Icon definido no Windows
										   // "IDI_INF..." Ícon de informação
	wcApp.hCursor = LoadCursor(NULL, IDC_ARROW);	// "hCursor" = handler do cursor (rato) 
							  // "NULL" = Forma definida no Windows
							  // "IDC_ARROW" Aspecto "seta" 
	wcApp.lpszMenuName = NULL;			// Classe do menu que a janela pode ter
							  // (NULL = não tem menu)
	wcApp.cbClsExtra = 0;				// Livre, para uso particular
	wcApp.cbWndExtra = 0;				// Livre, para uso particular
	wcApp.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
	// "hbrBackground" = handler para "brush" de pintura do fundo da janela. Devolvido por
	// "GetStockObject".Neste caso o fundo será branco

	// ============================================================================
	// 2. Registar a classe "wcApp" no Windows
	// ============================================================================
	if (!RegisterClassEx(&wcApp))
		return(0);

	// ============================================================================
	// 3. Criar a janela
	// ============================================================================
	hWnd = CreateWindow(
		szProgName,			// Nome da janela (programa) definido acima
		TEXT("Pipe Game"),// Texto que figura na barra do título
		WS_OVERLAPPED| WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,	// Estilo da janela (WS_OVERLAPPED= normal)
		(GetSystemMetrics(SM_CXSCREEN)- 600) /2,		// Posição x pixels (default=à direita da última)
		(GetSystemMetrics(SM_CYSCREEN)- 200) /2,		// Posição y pixels (default=abaixo da última)
		600,		// Largura da janela (em pixels)
		200,		// Altura da janela (em pixels)
		(HWND)HWND_DESKTOP,	// handle da janela pai (se se criar uma a partir de
						// outra) ou HWND_DESKTOP se a janela for a primeira, 
						// criada a partir do "desktop"
		(HMENU)NULL,			// handle do menu da janela (se tiver menu)
		(HINSTANCE)hInst,		// handle da instância do programa actual ("hInst" é 
						// passado num dos parâmetros de WinMain()
		0);				// Não há parâmetros adicionais para a janela

	  // ============================================================================
	  // 4. Mostrar a janela
	  // ============================================================================
	ShowWindow(hWnd, nCmdShow);	// "hWnd"= handler da janela, devolvido por 
					  // "CreateWindow"; "nCmdShow"= modo de exibição (p.e. 
					  // normal/modal); é passado como parâmetro de WinMain()
	UpdateWindow(hWnd);		// Refrescar a janela (Windows envia à janela uma 
					  // mensagem para pintar, mostrar dados, (refrescar)… 
	// ============================================================================
	// 5. Loop de Mensagens
	// ============================================================================
	// O Windows envia mensagens às janelas (programas). Estas mensagens ficam numa fila de
	// espera até que GetMessage(...) possa ler "a mensagem seguinte"	
	// Parâmetros de "getMessage":
	// 1)"&lpMsg"=Endereço de uma estrutura do tipo MSG ("MSG lpMsg" ja foi declarada no  
	//   início de WinMain()):
	//			HWND hwnd		handler da janela a que se destina a mensagem
	//			UINT message		Identificador da mensagem
	//			WPARAM wParam		Parâmetro, p.e. código da tecla premida
	//			LPARAM lParam		Parâmetro, p.e. se ALT também estava premida
	//			DWORD time		Hora a que a mensagem foi enviada pelo Windows
	//			POINT pt		Localização do mouse (x, y) 
	// 2)handle da window para a qual se pretendem receber mensagens (=NULL se se pretendem
	//   receber as mensagens para todas as
	// janelas pertencentes à thread actual)
	// 3)Código limite inferior das mensagens que se pretendem receber
	// 4)Código limite superior das mensagens que se pretendem receber

	// NOTA: GetMessage() devolve 0 quando for recebida a mensagem de fecho da janela,
	// 	  terminando então o loop de recepção de mensagens, e o programa 

	while (GetMessage(&lpMsg, NULL, 0, 0)) {
		TranslateMessage(&lpMsg);	// Pré-processamento da mensagem (p.e. obter código 
					   // ASCII da tecla premida)
		DispatchMessage(&lpMsg);	// Enviar a mensagem traduzida de volta ao Windows, que
					   // aguarda até que a possa reenviar à função de 
					   // tratamento da janela, CALLBACK TrataEventos (abaixo)
	}

	// ============================================================================
	// 6. Fim do programa
	// ============================================================================
	return((int)lpMsg.wParam);	// Retorna sempre o parâmetro wParam da estrutura lpMsg
}

// ============================================================================
// FUNÇÃO DE PROCESSAMENTO DA JANELA
// Esta função pode ter um nome qualquer: Apenas é necesário que na inicialização da
// estrutura "wcApp", feita no início de // WinMain(), se identifique essa função. Neste
// caso "wcApp.lpfnWndProc = WndProc"
//
// WndProc recebe as mensagens enviadas pelo Windows (depois de lidas e pré-processadas
// no loop "while" da função WinMain()
// Parâmetros:
//		hWnd	O handler da janela, obtido no CreateWindow()
//		messg	Ponteiro para a estrutura mensagem (ver estrutura em 5. Loop...
//		wParam	O parâmetro wParam da estrutura messg (a mensagem)
//		lParam	O parâmetro lParam desta mesma estrutura
//
// NOTA:Estes parâmetros estão aqui acessíveis o que simplifica o acesso aos seus valores
//
// A função EndProc é sempre do tipo "switch..." com "cases" que descriminam a mensagem
// recebida e a tratar.
// Estas mensagens são identificadas por constantes (p.e. 
// WM_DESTROY, WM_CHAR, WM_KEYDOWN, WM_PAINT...) definidas em windows.h
// ============================================================================



DWORD WINAPI readPipe(LPVOID a) {
	ControlData* cData = (ControlData*)a;
	TCHAR pipe_name[TAM];

	wsprintf(pipe_name, TEXT("\\\\.\\pipe\\CLIENTE_%d_READ"), cData->cliente->ID);

	if (!WaitNamedPipe(pipe_name, NMPWAIT_WAIT_FOREVER)) {
		exit(-1);
	}


	cData->readPipe = CreateFile(pipe_name, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (cData->readPipe == NULL) {
		exit(-1);
	}
	TCHAR buf[TAM];
	TCHAR mensagem[TAM];
	
	while (1) {
		if (!ReadFile(cData->readPipe, &cData->cliente->dados_jogo, sizeof(cData->cliente->dados_jogo), NULL, NULL)) {
			exit(-1);
		}
		switch (cData->cliente->dados_jogo.code)
		{
		case 1:_tprintf(TEXT("\nAgua começou a fluir"));
			break;
		case 2:		
			if (cData->cliente->dados_jogo.jogarCom != -2) {
				wsprintf(mensagem, TEXT("Perdeu o 1vs1"));
			}
			else {
				wsprintf(mensagem, TEXT("Perdeu o jogo"));
			}

			if (MessageBox(cData->hWnd, mensagem,
				TEXT("Sair"), MB_ICONWARNING | MB_OK) == IDOK) {
				wsprintf(buf, TEXT("quit\0"));
				if (!WriteFile(cData->writePipe, buf, (DWORD)_tcslen(buf) * sizeof(TCHAR), NULL, NULL)) {
					exit(-1);
				}
				DestroyWindow(cData->hWnd);
				exit(-1);
			}

			break;
		case 3:
			if (cData->cliente->dados_jogo.jogarCom == -2) {
				if (cData->cliente->dados_jogo.nivel == 5) {
					if (MessageBox(cData->hWnd, TEXT("Ganhou o jogo"),
						TEXT("Sair"), MB_ICONEXCLAMATION | MB_OK) == IDOK) {
						wsprintf(buf, TEXT("quit\0"));
						if (!WriteFile(cData->writePipe, buf, (DWORD)_tcslen(buf) * sizeof(TCHAR), NULL, NULL)) {
							exit(-1);
						}
						DestroyWindow(cData->hWnd);
						exit(-1);
					}
				}
				else {
					if (MessageBox(cData->hWnd, TEXT("Proximo Nivel ?"),
						TEXT("Ganhou o jogo"), MB_ICONQUESTION | MB_YESNO) != IDYES) {
						wsprintf(buf, TEXT("quit\0"));
						if (!WriteFile(cData->writePipe, buf, (DWORD)_tcslen(buf) * sizeof(TCHAR), NULL, NULL)) {
							exit(-1);
						}
					}
				}
			}
			else {
				if (MessageBox(cData->hWnd, TEXT("Ganhou o 1vs1"),
					TEXT("Sair"), MB_ICONEXCLAMATION | MB_OK) == IDOK) {
					wsprintf(buf, TEXT("quit\0"));
					if (!WriteFile(cData->writePipe, buf, (DWORD)_tcslen(buf) * sizeof(TCHAR), NULL, NULL)) {
						exit(-1);
					}
					DestroyWindow(cData->hWnd);
					exit(-1);
				}
			}

			break;
		default:
			break;
		}
		InvalidateRect(cData->hWnd, NULL, TRUE);
		Sleep(30);
	}
}

void writePipe(ControlData * cData) {
	cData->writePipe = NULL;
	TCHAR pipe_name[TAM];
	wsprintf(pipe_name, TEXT("\\\\.\\pipe\\CLIENTE_%d_WRITE"), cData->cliente->ID);

	if (!WaitNamedPipe(pipe_name, NMPWAIT_WAIT_FOREVER)) {
		exit(-1);
	}
	cData->writePipe = CreateFile(pipe_name, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (cData->writePipe == NULL) {
		exit(-1);
	}

}

void writeMemory(ControlData * cData, int* l, int* c) {
	TCHAR buf[TAM];
	wsprintf(buf, TEXT("add %d %d\0"), *l, *c);
	if (!WriteFile(cData->writePipe, buf, (DWORD)_tcslen(buf) * sizeof(TCHAR), NULL, NULL)) {
		return;
	}
}

DWORD WINAPI comecarCliente(LPVOID c) {
	ControlData* cData = (ControlData*)c;
	CLIENTE* cliente;
	cliente = cData->cliente;
	HANDLE hpipe;
	unsigned int i = 0;
	BOOL ret;
	DWORD n;
	TCHAR nome[TAM];
	wcscpy_s(nome, TAM, cliente->nome);
	nome[_tcslen(nome)] = '\0';


	if (!WaitNamedPipe(PIPE_NAME, NMPWAIT_WAIT_FOREVER)) {
		exit(-1);
	}

	hpipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hpipe == NULL) {
		exit(-1);
	}

	if (!ReadFile(hpipe, cliente, sizeof(*cliente), NULL, NULL)) {
		exit(-1);
	}
	
	wcscpy_s(cliente->nome, TAM, nome);
	cliente->ID = GetCurrentProcessId();
	if (!WriteFile(hpipe, cliente, sizeof(*cliente), &n, NULL))
		exit(-1);

	TCHAR semName[TAM];
	wsprintf(semName, READY_NAME, cliente->ID);
	cData->hReady = CreateSemaphore(NULL, 0, BUFFER_SIZE, semName);
	cData->pipe = &hpipe;

	WaitForSingleObject(cData->hReady, INFINITE);
	HANDLE threadRead = CreateThread(NULL, 0, readPipe, cData, 0, NULL); // iniciar thread que flui a agua
	writePipe(cData);

	WaitForSingleObject(threadRead, INFINITE);

	CloseHandle(hpipe);
	CloseHandle(threadRead);

}

void getTouchPos(ControlData* cData, int xPos, int yPos, int* l, int* c) {
	RECT rect;
	GetClientRect(cData->hWnd, &rect);
	int x1 = (rect.right / (cData->cliente->dados_jogo.tam_x + 2)), x2 = x1 + (rect.right / (cData->cliente->dados_jogo.tam_y + 2));
	int y1 = (rect.bottom / (cData->cliente->dados_jogo.tam_y + 2)) + 50, y2 = y1 + (rect.bottom / (cData->cliente->dados_jogo.tam_y + 2));
	*l = -1;
	*c = -1;
	for (int i = 0; i < cData->cliente->dados_jogo.tam_y; i++)
	{
		for (int j = 0; j < cData->cliente->dados_jogo.tam_x; j++) {
			if (xPos < x2 && yPos < y2 && xPos > x1 && yPos > y1) {
				*l = i;
				*c = j;
				return;
			}
			x1 = x2;
			x2 = x2 + (rect.right / (cData->cliente->dados_jogo.tam_x + 2));
		}
		y1 = y2;
		y2 = y2 + (rect.bottom / (cData->cliente->dados_jogo.tam_y + 2));
		x1 = (rect.right / (cData->cliente->dados_jogo.tam_x + 2)) , x2 = x1 + (rect.right / (cData->cliente->dados_jogo.tam_x + 2));;
	}
}

void adicionaBMP(HWND hWnd, HDC * bmpDC, BITMAP  * bmp,int * cano, int x1, int y1, int cx, int cy) {
	TCHAR canos[][TAM] = {_T("pipes_bmp/reto_esquerda_direita.bmp"), 
		_T("pipes_bmp/reto_cima_baixo.bmp"), 
		_T("pipes_bmp/curva_baixo_direita.bmp"),
		_T("pipes_bmp/curva_baixo_esquerda.bmp"), 
		_T("pipes_bmp/curva_cima_esquerda.bmp"), 
		_T("pipes_bmp/curva_cima_direita.bmp"), 
		_T("pipes_bmp/parede.bmp") };

	RECT rect;
	HBITMAP hBmp; 
	HBITMAP g_hbmMask;

	HDC hdc;

	hdc = GetDC(hWnd);
	*bmpDC = CreateCompatibleDC(hdc);
	hBmp = (HBITMAP)LoadImage(NULL, canos[*cano], IMAGE_BITMAP, cx - 2, cy - 2, LR_LOADFROMFILE);

	GetObject(hBmp, sizeof(*bmp), &*bmp);

	SelectObject(*bmpDC, hBmp);
	BitBlt(hdc, x1, y1, bmp->bmWidth, bmp->bmHeight, *bmpDC, 0, 0, SRCCOPY);

	
	ReleaseDC(hWnd, hdc);
	DeleteDC(*bmpDC);
}

LRESULT CALLBACK TrataEventos(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	static CLIENTE cliente;
	static ControlData cData;
	static HANDLE nameBox;
	static HANDLE nameLabel;
	static HANDLE nameButton;
	static HANDLE opButton1, opButton2, opButton3;
	static HANDLE waitLabel;
	static TCHAR tecla[10];
	static boolean game = FALSE;
	static TCHAR jogadas[][3] = { _T(" "), _T("━"), _T("┃"), _T("┏"), _T("┓"), _T("┛"), _T("┗"), _T("□") };
	
	static HDC bmpDC;
	static BITMAP bmp;
	HBITMAP hBmp;
	HBITMAP g_hbmBall = NULL, g_hbmMask = NULL;
	static HDC aguaDC = NULL;
	
	static int xBitmap;
	static int yBitmap;

	TCHAR buf[TAM];;
	static HANDLE hMutex;

	static int salto;
	boolean first = TRUE;

	HDC hdc;
	RECT rect;
	int xPos;
	int yPos;
	int l = 0;
	int c = 0;
	PAINTSTRUCT ps;
	int op = 0;

	static HDC memDC = NULL;
	switch (messg) {
	case WM_CREATE:
		GetClientRect(hWnd, &rect);
		cData.hdc = &hdc;
		cData.cliente = &cliente;
		cData.hWnd = hWnd;
		cData.memDC = &memDC;
		cData.aguaDC = &aguaDC;
		cData.hMutex = hMutex = CreateMutex(NULL, FALSE, NULL);

		nameLabel = CreateWindow(TEXT("STATIC"),
			TEXT("NOME:"),
			WS_CHILD | WS_VISIBLE,
			rect.right/2-25, 10, 50, 20,
			hWnd,
			NULL,
			NULL,
			NULL);
		nameBox = CreateWindow(TEXT("EDIT"),
			TEXT(""),
			WS_BORDER | WS_CHILD | WS_VISIBLE,
			50, 40, rect.right-100, 20,
			hWnd,
			NULL,
			NULL,
			NULL);
		nameButton = CreateWindow(TEXT("BUTTON"),
			TEXT("Inserir"),
			WS_BORDER | WS_CHILD | WS_VISIBLE,
			rect.right / 2 - 35, 70, 70, 20,
			hWnd,
			(HMENU) 1,
			NULL,
			NULL);
		break;
	case WM_CLOSE:
		if (MessageBox(hWnd, TEXT("Tem a certeza que quer sair?"),
			TEXT("Sair"), MB_ICONQUESTION | MB_YESNO | MB_HELP) == IDYES) {
			DestroyWindow(hWnd);
		}
		break;

	case WM_COMMAND:
		switch (wParam)
		{
		case 1:
			if (GetWindowText(nameBox, cData.cliente->nome, TAM) != 0) {
				CreateThread(NULL, 0, comecarCliente, &cData, 0, NULL);
				SetWindowPos(
					hWnd,
					NULL,
					(GetSystemMetrics(SM_CXSCREEN) - 700) / 2,
					(GetSystemMetrics(SM_CYSCREEN) - 700) / 2,
					700,
					700,
					NULL
				);
				GetClientRect(hWnd, &rect);
				ShowWindow(nameLabel, SW_HIDE);
				ShowWindow(nameBox, SW_HIDE);
				ShowWindow(nameButton, SW_HIDE);

				opButton1 = CreateWindow(TEXT("BUTTON"),
					TEXT("Jogar"),
					WS_BORDER | WS_CHILD | WS_VISIBLE,
					0, (rect.right / 8), rect.right, 80,
					hWnd,
					(HMENU)2,
					NULL,
					NULL);

				opButton2 = CreateWindow(TEXT("BUTTON"),
					TEXT("1vs1"),
					WS_BORDER | WS_CHILD | WS_VISIBLE,
					0, (rect.right / 8) * 3, rect.right, 80,
					hWnd,
					(HMENU)3,
					NULL,
					NULL);

				opButton3 = CreateWindow(TEXT("BUTTON"),
					TEXT("Sair"),
					WS_BORDER | WS_CHILD | WS_VISIBLE,
					0, (rect.right / 8) * 5, rect.right, 80,
					hWnd,
					(HMENU)4,
					NULL,
					NULL);
				//UpdateWindow(hWnd);
				//InvalidateRect(hWnd, NULL, TRUE);
			}
			break;
		case 2:
			ShowWindow(opButton1, SW_HIDE);
			ShowWindow(opButton2, SW_HIDE);
			ShowWindow(opButton3, SW_HIDE);

			wsprintf(buf, TEXT("start\0"));
			if (!WriteFile(cData.writePipe, buf, (DWORD)_tcslen(buf) * sizeof(TCHAR), NULL, NULL)) {
				return;
			}
			GetClientRect(hWnd, &rect);
			//inicializaBMP(hWnd, &bmpDC, &bmp, (rect.right / (cData.cliente->dados_jogo.tam_x + 2)), (rect.bottom / (cData.cliente->dados_jogo.tam_y + 2)));

			game = TRUE;
			InvalidateRect(hWnd, NULL, TRUE); //chama o WM_PAINT
			break;
		case 3:
			GetClientRect(hWnd, &rect);
			ShowWindow(opButton1, SW_HIDE);
			ShowWindow(opButton2, SW_HIDE);
			ShowWindow(opButton3, SW_HIDE);

			waitLabel = CreateWindow(TEXT("STATIC"),
				TEXT("Esperando outro jogador"),
				WS_CHILD | WS_VISIBLE,
				rect.right / 2 - 80, 10, 160, 20,
				hWnd,
				NULL,
				NULL,
				NULL);
			UpdateWindow(hWnd);
			wsprintf(buf, TEXT("link"));
			if (!WriteFile(cData.writePipe, buf, (DWORD)_tcslen(buf) * sizeof(TCHAR), NULL, NULL)) {
				return;
			}
			while (cData.cliente->dados_jogo.jogarCom == -2) {
				Sleep(1000);
			}

			ShowWindow(waitLabel, SW_HIDE);
			GetClientRect(hWnd, &rect);
			//inicializaBMP(hWnd, &bmpDC, &bmp, (rect.right / (cData.cliente->dados_jogo.tam_x + 2)), (rect.bottom / (cData.cliente->dados_jogo.tam_y + 2)));

			game = TRUE;
			InvalidateRect(hWnd, NULL, TRUE); //chama o WM_PAINT

			break;

		case 4:
			//sair ao fechar tudo
			cData.cliente->dados_jogo.shutdown = 1;
			wsprintf(buf, TEXT("quit"));
			if (!WriteFile(cData.writePipe, buf, (DWORD)_tcslen(buf) * sizeof(TCHAR), NULL, NULL)) {
				exit(-1);
			}

			break;
		}
		break;
		/*
	case WM_CHAR:
		tecla[0] = (TCHAR)wParam;
		break;
		*/
	case WM_LBUTTONDOWN:
		xPos = GET_X_LPARAM(lParam);
		yPos = GET_Y_LPARAM(lParam);
		getTouchPos(&cData, xPos, yPos, &l, &c);
		if (l != -1 && c != -1) {
			writeMemory(&cData, &l, &c);
		}
		
		/*
		hdc = GetDC(hWnd);
		GetClientRect(hWnd, &rect);
		SetTextColor(hdc, RGB(0, 0, 0));
		//SetBkMode(hdc, TRANSPARENT);
		rect.left = xPos;
		rect.top = yPos;
		wsprintf(tecla, TEXT("%d %d"), l, c);
		//DrawText(hdc, tecla, 1, &rect, DT_SINGLELINE | DT_NOCLIP);
		TextOut(hdc, 5, 5, tecla, _tcslen(tecla));
		ReleaseDC(hWnd, hdc);*/
		//InvalidateRect(hWnd, NULL, TRUE); //chama o WM_PAINT
		break;
	case WM_DESTROY:	// Destruir a janela e terminar o programa 
		wsprintf(buf, TEXT("quit"));
		if (!WriteFile(cData.writePipe, buf, (DWORD)_tcslen(buf) * sizeof(TCHAR), NULL, NULL)) {
			exit(-1);
		}				// "PostQuitMessage(Exit Status)"		
		PostQuitMessage(0);
		break;
	case WM_ERASEBKGND:
		return 1;

	case WM_PAINT: {

		hdc = BeginPaint(hWnd, &ps);

		RECT auxRect;
		GetClientRect(hWnd, &rect);

		if (aguaDC == NULL) {
			aguaDC = CreateCompatibleDC(hdc);
			hBmp = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
			SelectObject(aguaDC, hBmp);
			DeleteObject(hBmp);
			FillRect(aguaDC, &rect, CreateSolidBrush(RGB(0, 0, 255)));


			/*
			//NÃO APAGAR ISTO
			for (int i = 0; i < 2; i++)
			{
				rect.right = 100*(i+1);
				rect.bottom = 100*(i+1);
				BitBlt(memDC, i*100, i * 100, rect.right, rect.bottom, aguaDC, 0, 0, SRCCOPY);
				
			}*/
		}
		FillRect(hdc, &rect, CreateSolidBrush(RGB(255, 255, 255)));
		/*
		BitBlt(*dados->memDC, *dados->xBitmap, *dados->yBitmap,
			dados->bmp.bmWidth, dados->bmp.bmHeight, dados->bmpDC, 0, 0, SRCCOPY);// we can call BitBlt() to copy the image from our Memory DC to the Window DC, thus displaying on the screen
			//SRCCOPY - Copies the source rectangle directly to the destination rectangle.
			*/
		if (game) {

			GetClientRect(hWnd, &rect);
			GetClientRect(hWnd, &auxRect);
			if (cliente.dados_jogo.jogarCom != -2) {
				ZeroMemory(buf, TAM);
				wsprintf(buf, TEXT("A Jogar conta o cliente %d"), cliente.dados_jogo.jogarCom+1);
				TextOut(hdc, (rect.right / 4)*3, (rect.bottom / (cData.cliente->dados_jogo.tam_y + 2)) / 2, buf, _tcslen(buf));
			}
			else {
				ZeroMemory(buf, TAM);
				wsprintf(buf, TEXT("Nível %d"), cliente.dados_jogo.nivel);
				TextOut(hdc, (rect.right / 6) * 5, (rect.bottom / (cData.cliente->dados_jogo.tam_y + 2)) / 2, buf, _tcslen(buf));
			}
			TextOut(hdc, 0, (rect.bottom / (cData.cliente->dados_jogo.tam_y + 2)) / 2, TEXT("Jogadas Possiveis: "), _tcslen(TEXT("Jogadas Possiveis: ")));
			int x1 = (rect.right / 4), x2 = x1 + (rect.right / (cData.cliente->dados_jogo.tam_x + 2));
			TextOut(hdc, (rect.right / 4), (rect.bottom / (cData.cliente->dados_jogo.tam_y + 2)), TEXT("↑"), _tcslen(TEXT("↑")));
			for (int i = 0; i < 6; i++)
			{
				Rectangle(hdc, x1, 0, x2, (rect.bottom / (cData.cliente->dados_jogo.tam_y + 2)));
				
				int pos = cData.cliente->dados_jogo.jogadas[i] - 1;
				adicionaBMP(hWnd, &bmpDC, &bmp, &pos, x1 + 1, 1, (rect.right / (cData.cliente->dados_jogo.tam_x + 2)), (rect.bottom / (cData.cliente->dados_jogo.tam_y + 2)));
				x1 = x2;
				x2 = x2 + (rect.right / (cData.cliente->dados_jogo.tam_x + 2));
			}
			
			x1 = (rect.right / (cData.cliente->dados_jogo.tam_x + 2)), x2 = x1 + (rect.right / (cData.cliente->dados_jogo.tam_x + 2));
			int y1 = (rect.bottom / (cData.cliente->dados_jogo.tam_y + 2))+50, y2 = y1 + (rect.bottom / (cData.cliente->dados_jogo.tam_y + 2));
			for (int i = 0; i < cData.cliente->dados_jogo.tam_y; i++) {
				for (int j = 0; j < cData.cliente->dados_jogo.tam_x; j++) {
					Rectangle(hdc, x1, y1, x2, y2);
					//TextOut(hdc, x1+1, y1+1, jogadas[cData.cliente->dados_jogo.mapa[i][j].cano_pos], _tcslen(jogadas[cData.cliente->dados_jogo.mapa[i][j].cano_pos]));
					int pos = cData.cliente->dados_jogo.mapa[i][j].cano_pos - 1;
					if (pos != -1) {


						//BitBlt(hdc, x1 + 1, y1 + 1, bmp[pos].bmWidth, bmp[pos].bmHeight, bmpDC[pos], 0, 0, SRCCOPY);
						adicionaBMP(hWnd, &bmpDC, &bmp, &pos, x1 + 1, y1 + 1, (rect.right / (cData.cliente->dados_jogo.tam_x + 2)), (rect.bottom / (cData.cliente->dados_jogo.tam_y + 2)));
						if (cData.cliente->dados_jogo.mapa[i][j].agua == 1) {
							//auxRect.right = 100 * (i + 1);
							//auxRect.bottom = 100 * (i + 1);
							BitBlt(hdc, x1 + 1, y1 + 1, bmp.bmWidth, bmp.bmHeight, aguaDC, 0, 0, SRCPAINT);

							//FillRect(hdc, &rect, CreateSolidBrush(RGB(255, 255, 255)));
						}
					}
					x1 = x2;
					x2 = x2 + (rect.right / (cData.cliente->dados_jogo.tam_x + 2));
				}
				y1 = y2;
				y2 = y2 + (rect.bottom / (cData.cliente->dados_jogo.tam_y + 2));
				x1 = (rect.right / (cData.cliente->dados_jogo.tam_x + 2)), x2 = x1 + (rect.right / (cData.cliente->dados_jogo.tam_x + 2));
			}

		}
		//ReleaseDC(hWnd, memDC);
		//BitBlt(hdc, 0, 0, rect.right, rect.bottom, memDC, 0, 0, SRCCOPY);
		//ReleaseDC(hWnd, hdc);
		EndPaint(hWnd, &ps);

		

		
		break;
	}
	default:
		// Neste exemplo, para qualquer outra mensagem (p.e. "minimizar","maximizar","restaurar")
		// não é efectuado nenhum processamento, apenas se segue o "default" do Windows
		return(DefWindowProc(hWnd, messg, wParam, lParam));
		break;  // break tecnicamente desnecessário por causa do return
	}
	return(0);
}
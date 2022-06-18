#include <windows.h>
#include <tchar.h>
/* ===================================================== */
/* Programa base (esqueleto) para aplica��es Windows */
/* ===================================================== */
// Cria uma janela de nome "Janela Principal" e pinta fundo de branco
// Modelo para programas Windows:
// Composto por 2 fun��es:
// WinMain() = Ponto de entrada dos programas windows
// 1) Define, cria e mostra a janela
// 2) Loop de recep��o de mensagens provenientes do Windows
// TrataEventos()= Processamentos da janela (pode ter outro nome)
// 1) � chamada pelo Windows (callback)
// 2) Executa c�digo em fun��o da mensagem recebida
LRESULT CALLBACK TrataEventos(HWND, UINT, WPARAM, LPARAM);
// Nome da classe da janela (para programas de uma s� janela, normalmente este nome �
// igual ao do pr�prio programa) "szprogName" � usado mais abaixo na defini��o das
// propriedades do objecto janela
TCHAR szProgName[] = TEXT("Base");
// ============================================================================
// FUN��O DE IN�CIO DO PROGRAMA: WinMain()
// ============================================================================
// Em Windows, o programa come�a sempre a sua execu��o na fun��o WinMain()que desempenha
// o papel da fun��o main() do C em modo consola WINAPI indica o "tipo da fun��o" (WINAPI
// para todas as declaradas nos headers do Windows e CALLBACK para as fun��es de
// processamento da janela)
// Par�metros:
// hInst: Gerado pelo Windows, � o handle (n�mero) da inst�ncia deste programa
// hPrevInst: Gerado pelo Windows, � sempre NULL para o NT (era usado no Windows 3.1)
// lpCmdLine: Gerado pelo Windows, � um ponteiro para uma string terminada por 0
// destinada a conter par�metros para o programa
// nCmdShow: Par�metro que especifica o modo de exibi��o da janela (usado em
// ShowWindow()
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
	HWND hWnd; // hWnd � o handler da janela, gerado mais abaixo por CreateWindow()
	MSG lpMsg; // MSG � uma estrutura definida no Windows para as mensagens
	WNDCLASSEX wcApp; // WNDCLASSEX � uma estrutura cujos membros servem para
	// definir as caracter�sticas da classe da janela
		// ============================================================================
		// 1. Defini��o das caracter�sticas da janela "wcApp"
		// (Valores dos elementos da estrutura "wcApp" do tipo WNDCLASSEX)
		// ============================================================================
	wcApp.cbSize = sizeof(WNDCLASSEX); // Tamanho da estrutura WNDCLASSEX
	wcApp.hInstance = hInst; // Inst�ncia da janela actualmente exibida
	// ("hInst" � par�metro de WinMain e vem
	// inicializada da�)
	wcApp.lpszClassName = szProgName; // Nome da janela (neste caso = nome do programa)
	wcApp.lpfnWndProc = TrataEventos; // Endere�o da fun��o de processamento da janela
	// ("TrataEventos" foi declarada no in�cio e
	// encontra-se mais abaixo)
	wcApp.style = CS_HREDRAW | CS_VREDRAW; // Estilo da janela: Fazer o redraw se for
	// modificada horizontal ou verticalmente
	wcApp.hIcon = LoadIcon(NULL, IDI_APPLICATION); // "hIcon" = handler do �con normal
	// "NULL" = Icon definido no Windows
	// "IDI_AP..." �cone "aplica��o"
	wcApp.hIconSm = LoadIcon(NULL, IDI_INFORMATION); // "hIconSm" = handler do �con pequeno
	// "NULL" = Icon definido no Windows
	// "IDI_INF..." �con de informa��o
	wcApp.hCursor = LoadCursor(NULL, IDC_ARROW); // "hCursor" = handler do cursor (rato)
   // "NULL" = Forma definida no Windows
   // "IDC_ARROW" Aspecto "seta"
	wcApp.lpszMenuName = NULL; // Classe do menu que a janela pode ter
   // (NULL = n�o tem menu)
	wcApp.cbClsExtra = 0; // Livre, para uso particular
	wcApp.cbWndExtra = 0; // Livre, para uso particular
	wcApp.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	// "hbrBackground" = handler para "brush" de pintura do fundo da janela. Devolvido por
	// "GetStockObject".Neste caso o fundo ser� branco
	// ============================================================================
	// 2. Registar a classe "wcApp" no Windows
	// ============================================================================
	if (!RegisterClassEx(&wcApp))
		return(0);
	// ============================================================================
	// 3. Criar a janela
	// ============================================================================
	hWnd = CreateWindow(
		szProgName, // Nome da janela (programa) definido acima
		TEXT("Exemplo de Janela Principal em C"), // Texto que figura na barra do t�tulo
		WS_OVERLAPPEDWINDOW, // Estilo da janela (WS_OVERLAPPED= normal)
		CW_USEDEFAULT, // Posi��o x pixels (default=� direita da �ltima)
		CW_USEDEFAULT, // Posi��o y pixels (default=abaixo da �ltima)
		CW_USEDEFAULT, // Largura da janela (em pixels)
		CW_USEDEFAULT, // Altura da janela (em pixels)
		(HWND)HWND_DESKTOP, // handle da janela pai (se se criar uma a partir de
		// outra) ou HWND_DESKTOP se a janela for a primeira,
		// criada a partir do "desktop"
		(HMENU)NULL, // handle do menu da janela (se tiver menu)
		(HINSTANCE)hInst, // handle da inst�ncia do programa actual ("hInst" �
		// passado num dos par�metros de WinMain()
		0); // N�o h� par�metros adicionais para a janela
		// ============================================================================
		// 4. Mostrar a janela
		// ============================================================================
	ShowWindow(hWnd, nCmdShow); // "hWnd"= handler da janela, devolvido por
   // "CreateWindow"; "nCmdShow"= modo de exibi��o (p.e.
   // normal/modal); � passado como par�metro de WinMain()
	UpdateWindow(hWnd); // Refrescar a janela (Windows envia � janela uma
   // mensagem para pintar, mostrar dados, (refrescar)�
   // ============================================================================
   // 5. Loop de Mensagens
   // ============================================================================
   // O Windows envia mensagens �s janelas (programas). Estas mensagens ficam numa fila de
   // espera at� que GetMessage(...) possa ler "a mensagem seguinte"
   // Par�metros de "getMessage":
   // 1)"&lpMsg"=Endere�o de uma estrutura do tipo MSG ("MSG lpMsg" ja foi declarada no
   // in�cio de WinMain()):
   // HWND hwnd handler da janela a que se destina a mensagem
   // UINT message Identificador da mensagem
   // WPARAM wParam Par�metro, p.e. c�digo da tecla premida
   // LPARAM lParam Par�metro, p.e. se ALT tamb�m estava premida
   // DWORD time Hora a que a mensagem foi enviada pelo Windows
   // POINT pt Localiza��o do mouse (x, y)
   // 2)handle da window para a qual se pretendem receber mensagens (=NULL se se pretendem
   // receber as mensagens para todas as janelas pertencentes � thread actual)
   // 3)C�digo limite inferior das mensagens que se pretendem receber
   // 4)C�digo limite superior das mensagens que se pretendem receber
   // NOTA: GetMessage() devolve 0 quando for recebida a mensagem de fecho da janela,
   // terminando ent�o o loop de recep��o de mensagens, e o programa
	while (GetMessage(&lpMsg, NULL, 0, 0)) {
		TranslateMessage(&lpMsg); // Pr�-processamento da mensagem (p.e. obter c�digo
	   // ASCII da tecla premida)
		DispatchMessage(&lpMsg); // Enviar a mensagem traduzida de volta ao Windows, que
	   // aguarda at� que a possa reenviar � fun��o de
	   // tratamento da janela, CALLBACK TrataEventos (abaixo)
	}
	// ============================================================================
	// 6. Fim do programa
	// ============================================================================
	return((int)lpMsg.wParam); // Retorna sempre o par�metro wParam da estrutura lpMsg
}
// ============================================================================
// FUN��O DE PROCESSAMENTO DA JANELA
// Esta fun��o pode ter um nome qualquer: Apenas � neces�rio que na inicializa��o da
// estrutura "wcApp", feita no in�cio de WinMain(), se identifique essa fun��o. Neste
// caso "wcApp.lpfnWndProc = WndProc"
//
// WndProc recebe as mensagens enviadas pelo Windows (depois de lidas e pr�-processadas
// no loop "while" da fun��o WinMain()
// Par�metros:
// hWnd O handler da janela, obtido no CreateWindow()
// messg Ponteiro para a estrutura mensagem (ver estrutura em 5. Loop...
// wParam O par�metro wParam da estrutura messg (a mensagem)
// lParam O par�metro lParam desta mesma estrutura
//
// NOTA:Estes par�metros est�o aqui acess�veis o que simplifica o acesso aos seus valores
//
// A fun��o EndProc existe um "switch..." com "cases" que descriminam a mensagem
// recebida e a tratar. Estas mensagens s�o identificadas por constantes (p.e.
// WM_DESTROY, WM_CHAR, WM_KEYDOWN, WM_PAINT...) definidas em windows.h
// ============================================================================
typedef struct {
	HANDLE hMutex;
	int* xBitmap;
	int* yBitmap;
	int* limDir;
	int* salto;
	HWND hWnd;
	BITMAP bmp;
	HDC bmpDC;
	HDC* memDC;
} DadosThread;

DWORD WINAPI MovimentaImagem(LPVOID lparam)
{
	DadosThread* dados = (DadosThread*)lparam;
	RECT rect;
	int sentido = 1;

	while (1)
	{
		WaitForSingleObject(dados->hMutex, INFINITE);

		if (*dados->salto > 0)
		{
			*dados->xBitmap = *dados->xBitmap + (sentido * *dados->salto);

			if (*dados->xBitmap <= 0)
			{
				*dados->xBitmap = 0;
				sentido = 1;
			}
			else if (*dados->xBitmap >= *dados->limDir)
			{
				*dados->xBitmap = *dados->limDir;
				sentido = -1;
			}

			if (*dados->memDC != NULL)
			{
				GetClientRect(dados->hWnd, &rect); //dimens�es 
				FillRect(*dados->memDC, &rect, CreateSolidBrush(RGB(255, 0, 0)));//The FillRect function fills a rectangle by using the specified brush. This function includes the left and top borders, but excludes the right and bottom borders of the rectangle.
				BitBlt(*dados->memDC, *dados->xBitmap, *dados->yBitmap,
					dados->bmp.bmWidth, dados->bmp.bmHeight, dados->bmpDC, 0, 0, SRCCOPY);// we can call BitBlt() to copy the image from our Memory DC to the Window DC, thus displaying on the screen
					//SRCCOPY - Copies the source rectangle directly to the destination rectangle.
			}
		}

		ReleaseMutex(dados->hMutex);

		InvalidateRect(dados->hWnd, NULL, TRUE);
		Sleep(30);
	}
}

LRESULT CALLBACK TrataEventos(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	RECT rect;
	PAINTSTRUCT ps;
	MINMAXINFO* mmi;

	static HDC bmpDC;
	HBITMAP hBmp;
	static BITMAP bmp;
	static int xBitmap;
	static int yBitmap;
	static int limDir;
	static HANDLE hMutex;
	static DadosThread dados;
	static int salto;

	static HDC memDC = NULL;

	switch (messg)
	{
	case WM_CREATE:
		hBmp = (HBITMAP)LoadImage(NULL, TEXT("transferir.bmp"), IMAGE_BITMAP, 100, 100, LR_LOADFROMFILE);
		GetObject(hBmp, sizeof(bmp), &bmp);

		hdc = GetDC(hWnd);
		bmpDC = CreateCompatibleDC(hdc);
		SelectObject(bmpDC, hBmp);
		ReleaseDC(hWnd, hdc);

		GetClientRect(hWnd, &rect);
		xBitmap = (rect.right / 2) - (bmp.bmWidth / 2);
		yBitmap = (rect.bottom / 2) - (bmp.bmHeight / 2);
		limDir = rect.right - bmp.bmWidth;
		salto = 2;

		hMutex = CreateMutex(NULL, FALSE, NULL);
		dados.hMutex = hMutex;
		dados.hWnd = hWnd;
		dados.limDir = &limDir;
		dados.xBitmap = &xBitmap;
		dados.yBitmap = &yBitmap;
		dados.salto = &salto;
		dados.bmp = bmp;
		dados.bmpDC = bmpDC;
		dados.memDC = &memDC;

		CreateThread(NULL, 0, MovimentaImagem, &dados, 0, NULL);
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		GetClientRect(hWnd, &rect);

		if (memDC == NULL) //primeira vez
		{
			memDC = CreateCompatibleDC(hdc);
			hBmp = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
			SelectObject(memDC, hBmp);
			DeleteObject(hBmp);
			FillRect(memDC, &rect, CreateSolidBrush(RGB(255, 0, 0)));
			BitBlt(memDC, xBitmap, yBitmap, bmp.bmWidth, bmp.bmHeight, bmpDC, 0, 0, SRCCOPY);
			//SRCCOPY - Copies the source rectangle directly to the destination rectangle.
		}

		BitBlt(hdc, 0, 0, rect.right, rect.bottom, memDC, 0, 0, SRCCOPY);

		EndPaint(hWnd, &ps);
		break;

	case WM_ERASEBKGND:
		return 1;

	case WM_GETMINMAXINFO:
		mmi = (MINMAXINFO*)lParam;
		WaitForSingleObject(hMutex, INFINITE);
		mmi->ptMinTrackSize.x = bmp.bmWidth + 2;
		mmi->ptMinTrackSize.y = bmp.bmHeight * 2;
		ReleaseMutex(hMutex);
		break;

	case WM_SIZE:
		WaitForSingleObject(hMutex, INFINITE);
		xBitmap = (LOWORD(lParam) / 2) - (bmp.bmWidth / 2);
		yBitmap = (HIWORD(lParam) / 2) - (bmp.bmHeight / 2);
		limDir = LOWORD(lParam) - bmp.bmWidth;
		memDC = NULL;
		ReleaseMutex(hMutex);
		break;

	case WM_CLOSE:
		if (MessageBox(hWnd, TEXT("Tem a certeza que quer sair?"),
			TEXT("Confirma��o"), MB_ICONQUESTION | MB_YESNO) == IDYES)
		{
			DestroyWindow(hWnd);
		}
		break;
	case WM_DESTROY: // Destruir a janela e terminar o programa 
	// "PostQuitMessage(Exit Status)"
		PostQuitMessage(0);
		break;
	default:
		// Neste exemplo, para qualquer outra mensagem (p.e. "minimizar","maximizar","restaurar")
		// n�o � efectuado nenhum processamento, apenas se segue o "default" do Windows
		return(DefWindowProc(hWnd, messg, wParam, lParam));
		break;  // break tecnicamente desnecess�rio por causa do return
	}
	return(0);
}
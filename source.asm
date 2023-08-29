.code
;argumenty:
;po wejsciu do funkcjie argumenty mamy
;rcx		- adres tablicy wejœciowej
;rdx		- adres tablicy wynikowej
;r8			- rozmiar wiersza obrazu w bajtach z paddingiem
;r9			- numer wiersza obrazu poczatkowego do przefiltorowania
;stos:		
;[rsp + 40] - numer wiersza obrazu koncowego do przefiltorowania
;[rsp + 48] - szerokowsc w pikselach
;[rsp + 56] - wysokosc w pikselach
bmp_filtering proc
	;umieszczenie argumentów na stosie
	push rbp		
	push r9			
	push r8			
	push rdx		
	push rcx	
	
	mov rbp, rsp	

	;miejsce na wektory pomocnicze: [rbp-16], [rbp-32], [rbp-48]
	sub rsp, 16*3	

	push rsi
	push rdi

	mov esi, [rbp+24]	
	dec dword ptr [rbp+64+32]	;numer ostatniego wiersza
	dec dword ptr [rbp+56+32]	;numer ostatniego piksela
bmp_filtering0:
	cmp esi, [rbp+48+32]		;sprawdzenie czy bie¿¹cy wiersz jest wiêkszy ni¿ wiersz koñcowy do filtorwania
	ja	bmp_filtering_ret	

	cmp esi, 0					;nie filtrujemy pierwszego wiersza bitmapy
	je	bmp_filtering2
	cmp esi, [rbp+64+32]		;nie filtrujemy ostatniego wierszsa bitmapy
	je	bmp_filtering2


	mov edi, 0				
bmp_filtering1:
	cmp edi, [rbp+56+32]		;sprawdzenie czy bie¿¹cy piksel jest wiekszy niz ostani piksel do filtorwania
	ja	bmp_filtering2		

	cmp edi, 0				;nie filtrujemy pierwszego piksela w wierszu
	je	bmp_filtering3
	cmp edi, [rbp+56+32]	;nie filtrujemy ostatniego piksela w wierszu
	je	bmp_filtering3
	
	;jesli piksel mo¿na filtrowaæ:
	;obliczenie offsetu piksela: offset_pix = (row_size * row) + (3 * pix)
	xor rax,rax			
	mov eax, 3
	mul edi				
	mov ebx, eax		
	mov eax, [rbp+16]	
	mul esi				
	add eax, ebx		
	push rax			

	;obliczenie przesuniêcia piksela wzglêdem pocz¹tku bitmapy
	add rax, [rbp]			
	mov rdx, [rbp+16]		
	call bmp_filter_pixel	
	
	;po filtrowaniu:
	;ax - kolor B
	;bx - kolor G
	;cx - kolor R

	pop rax				;przywrócenie offsetu piksela do rax

	add rax, [rbp+8]
	mov [rax], cl		;blue
	mov [rax+1], ch		;green
	mov [rax+2], dl		;red

;zwiêkszenie licznika piksela
bmp_filtering3:
	inc edi
	jmp bmp_filtering1		

;przejœcie do kolejnego wiersza
bmp_filtering2:
	inc esi
	jmp bmp_filtering0

bmp_filtering_ret:
	pop rdi
	pop rsi
	add rsp, 16*3		;przywrócenie dawnego wskaŸnika stosu
	pop rcx
	pop rdx
	pop r8
	pop r9
	pop rbp				
	ret
bmp_filtering endp


;argumenty:
;rax - adres filtrowanego piksela
;rdx - rozmiar wiersza w bajtach
;zwraca:
;cl - kolor B po filtrowaniu
;ch - kolor G po filtrowaniu
;dl - kolor R po filtrowaniu
bmp_filter_pixel proc
	mov rbx, rax		 
	sub rbx, 3			;rbx - adres œrodkowego wiersza filtrowanego fragmentu
	
	mov rcx, rax		
	add rcx, rdx		;adres bie¿¹cego piksela + rozmiar wiersza	
	sub rcx, 3			;przesuniêcie do pocz¹tu wiersza
						;rcx - adres dolnego wiersza filtrowanego obrazu

	sub rax, rdx		;adres bie¿¹cego piksela - rozmiar wiersza
	sub rax, 3			;przesuniêcie do pocz¹tku wiersza	
						;rdx - adres górengo wiersza filtrowanego orbazu
	mov rdx, rax


	PXOR xmm0, xmm0					
	MOVDQU xmm1, [rdx]					;xmm1 - bajty górnego wiersza 	
	PUNPCKLBW xmm1, xmm0				
	MOVDQU xmm2, [rbx]					;xmm2 - bajty œrodkowego wiersza
	PUNPCKLBW xmm2, xmm0				
	MOVDQU xmm3, [rcx]					;xmm3 - bajty dolnego wiersza
	PUNPCKLBW xmm3, xmm0	
	
	MOVDQU xmm4, [offset lp_row1]		;xmm4 - górny wiersz filtra
	MOVDQU xmm5, [offset lp_row2]		;xmm5 - œrodkowey wiersz filtra
	MOVDQU xmm6, [offset lp_row3]		;xmm6 - dolny wiersz filtra

	;mno¿enie wierszy filtorwanego fragmentu z filtrem
	PMULLW xmm1, xmm4		 
	PMULLW xmm2, xmm5		 
	PMULLW xmm3, xmm6	

	PADDW xmm1, xmm2	
	PADDW xmm1, xmm3		;xmm1 - suma wartoœci wierszy

	MOVDQU [rbp-16], xmm1	;rbp-16 - wektor B G R B G R B G

	;obliczanie koloru R
	xor eax, eax
	mov al, [rdx + 8]		;al - kolor R z wiersza 0 i kolumny 2
	mov [rbp-32], ax		;sk³adowa R w wektorze pomocniczym
	mov al, [rbx + 8]		;al - kolor R z wiersza 1 i kolumny 2
	mov [rbp-32+2], ax
	mov al, [rcx + 8]		;al - kolor R z wiersza 2 i kolumny 2
	mov [rbp-32+4], ax
	mov ax, lp_row1[16]		;ax - kolor R z wiersza 0 i kolumny 2 filtru
	mov [rbp-48+0], ax
	mov ax, lp_row2[16]		;ax - kolor R z wiersza 1 i kolumny 2 filtru
	mov [rbp-48+2], ax
	mov ax, lp_row3[16]		;ax - kolor R z wiersza 2 i kolumny 2 filtru
	mov [rbp-48+4], ax

	;[rbp-32] kolory R z ostatniej kolumny macierzy obrazu
	;[rbp-48] kolory R z ostatniej kolumny macierzy filtru

	MOVDQU xmm1, [rbp-32]		
	MOVDQU xmm2, [rbp-48]		
	PMULLW xmm1, xmm2			
	MOVDQU [rbp-32], xmm1		
	
	;sumowanie bajtów kolorów R z wektora B G R... i wartoœci w wektora pomocniczego 
	mov ax, [rbp-16+4]
	add ax, [rbp-16+10]	
	add ax, [rbp-32+0]
	add ax, [rbp-32+2]
	add ax, [rbp-32+4]
	cwde			
	mov [rbp-32+8], eax		;suma w wektorze pomocniczym [rbp-32] na ostatniej pozycji (+8)

	;obliczanie koloru G
	mov ax, [rbp-16+2]
	add ax, [rbp-16+8]
	add ax, [rbp-16+14]
	cwde			
	mov [rbp-32+4], eax		;suma kolorów G wektora pomocniczego na œrodkowej pozycji (+4) wektora [rbp-32] 

	;obliczanie koloru B
	mov ax, [rbp-16]
	add ax, [rbp-16+6]
	add ax, [rbp-16+12]
	cwde					
	mov [rbp-32], eax		;suma kolorów B wektora pomocniczego na pocz¹tku (+0) wektora [rbp-32] 

	;konwersja wartoœci z wektora B G R - na floaty w celu wykonania dzielenia
	MOVDQU xmm1, [rbp-32]			;xmm1 - B G R
	MOVDQU xmm2, [offset weight]	;xmm2 - wagi z filtra
	CVTDQ2PS xmm2, xmm2				;konwersja do liczb zmienno pozycyjnych (w celu przyspieszenia dzielenia
	CVTDQ2PS xmm1, xmm1				
	DIVPS xmm1, xmm2				
	CVTPS2DQ xmm1, xmm1				;konwerjsa powrotna do liczb calkowitych
	
	;ograniczenie wartoœci aby zapobiec wyjœciu poza wartoœci, które mog¹ zmieœciæ siê w bajcie
	MOVDQU xmm2, [offset min]		
	PMAXSD xmm1, xmm2				;ograniczenie do minimum = 0

	MOVDQU xmm2, [offset max]		
	PMINSD xmm1, xmm2				;ograniczenie do maksimum = 255
	MOVDQU [rbp-32],xmm1			

	;pobranie kolorów
	mov cl,[rbp-32]			
	mov ch,[rbp-32+4]		
	mov dl,[rbp-32+8]		
	ret


lp_row1 dw		0, 0, 0,	-1, -1, -1,	0, 0,			0  
lp_row2 dw		-1, -1, -1,	4, 4, 4, -1, -1,			-1  
lp_row3	dw		0, 0, 0,	-1, -1, -1,	0, 0,			0  
weight	dd		1, 1, 1, 1
min	dd			0, 0, 0, 0
max	dd			255, 255, 255, 255

bmp_filter_pixel endp



end

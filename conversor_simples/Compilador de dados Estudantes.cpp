#include <stdio.h>
#include <stdlib.h>
#include <conio2.h>
#include <string.h>
#include <windows.h>
#include <time.h>

//Outras defini��es
#define QUANTDIG_NOME 60 //n�mero m�ximo de caracteres que podem ocupar os nomes
#define QUANTDIG_RG 10 //n�mero de d�gitos que ocupa o RG
#define QUANTDIG_INST 150//numero maximo de digitos que pode ter o omme da instiui��o de ensino
#define NUMALUNOS 700 //n�mero m�ximo de alunos que poder�o ser contabilizados
#define SEPARADOR 9 //caracter que separa os dados (nesse caso, o TAB
#define TAMLINHA 300 //tamanho maximo de uma linha

//Para debug
#define LOG_DIAS_SEMANA 0
#define LOG_GRAVACAO 0
#define LOG_LEITURA_ALUNOS 0
#define LOG_LEITURA_LINHA 0

struct aluno{
	unsigned char nome[QUANTDIG_NOME];
	char rg[QUANTDIG_RG];
	unsigned char instituicao[QUANTDIG_INST];
	int transporte[6][8];//Tabela de dados do transporte. 6 linhas s�o os dias da semana. Colunas s�o Ida e volta de cada turno.
	// 88 para marcado, 0 para desmarcado.
};
void sair(void){
	printf("\n\n--------------------------\nExecucao foi interrompida!\n--------------------------\n");
	getch();
	exit(0);
}

int main(void)
{
	unsigned char tempnome[QUANTDIG_NOME]={0};
	unsigned char linha[TAMLINHA]={0};
	unsigned char linhaanterior[TAMLINHA]={0};
	unsigned char instituicao[QUANTDIG_INST]={0};
	aluno alunos[NUMALUNOS]={0};//cria a estrutura para armazenar todos alunos
	int passo=0;//Armazena o passo em que estamos.
		//0 - AINDA NADA, BUSCAR PELO DIA DA SEMANA
		//1 - DIA DA SEMANA ENCONTRADO, BUSCAR AGORA O TURNO
		//2 - TURNO ENCONTRADO, BUSCAR AGORA PELOS ALUNOS
	int dia;//Armazena o dia da semana em quest�o
		//0 Segunda
		//1 Ter�a
		//...
		//5 S�bado
	int turno;//armazena o turno, sendo:
		//0 Manha
		//1 Tarde
		//2 Vespertino
		//3 Noite
	int x,y,z;//auxiliar
	int reg;//armazena a posi��o do registro que est� sendo editado, lido ou gravado
	FILE *fluxo;
    clrscr();
	printf("Para garantir que a fun��o funcione corretamente:\n\n    * Garanta que a lista de alunos seja de grupo unico (UCS, Cenecista, Cursos ou Barao);\n    * Os dados foram copiados do relatorio HTML do sistema da transportadora;\n    * Os dados estejam salvos em \"dados.txt\" neste diretorio;\n\n Pressione qualquer tecla para iniciar o processo...");
	printf("\n\n Para criar e abrir o arquivo pressione 'o'");
	tempnome[0]=getch();
	if(tempnome[0]=='o'){
		system("echo. > dados.txt");
		system("start dados.txt");
		printf("\n\n ARQUIVO ABERTO, PRESSIONE QUALQUER TECLA PARA INICIAR O PROCESSAMENTO");
		getch();
	}
	tempnome[0]=0;
	clrscr();
	printf("\nAbrindo arquivo de dados");
	if((fluxo=fopen("dados.txt","rt"))==NULL){
		printf("\n\nNAO FOI POSSIVEL ENCONTRAR O ARQUIVO! VERIFIQUE, E TENTE NOVAMENTE");
		getch();
		exit(0);
	}
	printf(" [OK]");
	while(!feof(fluxo)){//um loop por LINHA do arquivo(at� final do arquivo)
		if(LOG_LEITURA_LINHA)printf("\n Lendo nova linha");
		x=0;
		while(x<TAMLINHA){
			linhaanterior[x]=linha[x];
			x++;
		}
		x=0;
		while(x<TAMLINHA){
			linha[x]=0;
			x++;
		}
		x=-1;
		do{//l� uma linha completa (at� quebra de linha)
			x++;
			linha[x]=getc(fluxo);//l� um caracter do fluxo
		}while((linha[x]!='\n')&&(!feof(fluxo)));
		if(feof(fluxo)&&x==0) break;
		if(LOG_LEITURA_LINHA)printf(" [OK]");
		linha[x]=0;
		switch(passo){
			case 0://Buscar por dia, se n�o for localizado, ignora, e segue para proxima linha
				if(LOG_DIAS_SEMANA)printf("\n   Buscando por DIA");
				switch(linha[0]){
					case 'S'://Segunda, sexta ou s�bado
						switch(linha[2]){//compara TERCEIRA linha, pois a segunda � igual em mais de uma opcao
							case 'g'://segunda
								if(linha[7]==45){//se o oitavo digito � um hifem (verifica��o de segur�n�a)
									if(LOG_DIAS_SEMANA)printf(" - SEGUNDA");
									dia=0;
									passo++;
								}
								break;
							case 'x'://sexta
								if(linha[5]==45){//se o sexto digito � um hifem (verifica��o de segur�n�a)
									if(LOG_DIAS_SEMANA)printf(" - SEXTA");
									dia=4;
									passo++;
								}
								break;
							case 161://sabado
								if(linha[5]=='d'){//se o segundo digito � o � (verifica��o de segur�n�a)
									if(LOG_DIAS_SEMANA)printf(" - SABADO");
									dia=5;
									passo++;
								}
								break;
						}
						break;
					case 'T'://Ter�a-feira
						if(linha[6]==45){//se o sexto digito � um hifem (verifica��o de segur�n�a)
							if(LOG_DIAS_SEMANA)printf(" - TERCA");
							dia=1;
							passo++;
						}
						break;
					case 'Q'://quarta ou quinta
						switch(linha[2]){
							case 'a'://quarta
								if(linha[6]==45){//se o sexto s�timo � um hifem (verifica��o de segur�n�a)
									if(LOG_DIAS_SEMANA)printf(" - QUARTA");
									dia=2;
									passo++;
								}
								break;
							case 'i'://quinta
								if(linha[6]==45){//se o sexto s�timo � um hifem (verifica��o de segur�n�a)
									if(LOG_DIAS_SEMANA)printf(" - QUINTA");
									dia=3;
									passo++;
								}
								break;
						}
						break;
				}
				if(LOG_DIAS_SEMANA&&passo==0) printf(" [NON]");
				break;
			case 1://buscar por turno
				if(LOG_DIAS_SEMANA)printf("\n   Buscando por TURNO");
				switch(linha[0]){
					case 'M'://manh�
						if(linha[1]=='a'&&linha[2]=='n'&&linha[3]=='h'&&linha[6]==0){//verifica��o de seguran�a
							if(LOG_DIAS_SEMANA)printf("-   MANHA");
							turno=0;
							passo++;
						}
						break;
					case 'T'://tarde
						if(linha[1]=='a'&&linha[2]=='r'&&linha[3]=='d'&&linha[4]=='e'){//verifica��o de seguran�a
							if(LOG_DIAS_SEMANA)printf("-   TARDE");
							turno=1;
							passo++;
						}
						break;
					case 'V'://vespertino 16H
						if(linha[1]=='e'&&linha[2]=='s'&&linha[3]=='p'&&linha[12]=='6'){//verifica��o de seguran�a
							if(LOG_DIAS_SEMANA)printf("-   VESPERTINO");
							turno=2;
							passo++;
						}
						break;
					case 'N'://noite
						if(linha[1]=='o'&&linha[2]=='i'&&linha[3]=='t'&&linha[4]=='e'){//verifica��o de seguran�a
							if(LOG_DIAS_SEMANA)printf("-   NOITE");
							turno=3;
							passo++;
						}
						break;
				}
				if(LOG_DIAS_SEMANA&&passo==1) printf(" [NON]");
				break;
			case 2://busca a linha com "Nome" ou "Total"
				if(LOG_LEITURA_ALUNOS)printf("\nLinha Ignorada   Buscando inicio de lista");
				if(linha[0]=='N'&&linha[1]=='o'&&linha[2]=='m'&&linha[3]=='e'&&linha[4]==SEPARADOR){//se encontrou este a palavra "nome", segue para a proxima institui��o
					x=0;
					while(x<QUANTDIG_INST){
						instituicao[x]=linhaanterior[x];
						x++;
					}
					passo++;
				}
				if(linha[0]=='T'&&linha[1]=='o'&&linha[2]=='t'&&linha[3]=='a'&&linha[4]=='l'){//encontrou a palavra total (seve seguir para proxima institui��o
					if(turno==3)passo=0;//concluiu a grava��o dos alunos deste dia
					else passo=1;//retorna dois passos, faltam mais turnos do mesmo dia.
				}
				break;
			case 3://preencher dados do aluno ou verificar final de dados, e zerar o passo.
				if(LOG_LEITURA_ALUNOS)printf("\n   Coletando dados de aluno");
				if(linha[0]=='T'&&linha[1]=='o'&&linha[2]=='t'&&linha[3]=='a'&&linha[4]=='l'){//encontrou a palavra total (seve seguir para proxima institui��o
					passo=2;
					break;
				}
				x=0;
				if(LOG_LEITURA_ALUNOS)printf("\n   Coletando nome de aluno");
				while(x<QUANTDIG_NOME){//grava o nome do indiv�duo numa variavel tempor�ria
					tempnome[x]=0;
					x++;
				}
				x=0;
				while(linha[x]!=SEPARADOR){//grava o nome do indiv�duo numa variavel tempor�ria
					tempnome[x]=linha[x];
					x++;
					//printf(" %c", linha[x-1]); Sleep(400);
				}
				do{
					x++;
				}while(linha[x]!=SEPARADOR);//PULA O CAMPO DO EMAIL
				reg=y=0;
				if(linha[x]!=SEPARADOR){
					if(LOG_LEITURA_ALUNOS)printf("\nERRO ENS00\n          LINHA: %s\n          LINHA[%d]: %c", linha, x,linha[x]);
					sair();
				}//Precisa ter um separador aqui
				x++;//avan�a um, pra deixar em cima do primeiro digito do RG
				while(linha[x+y]!=SEPARADOR&&y<10){
					if(alunos[reg].rg[0]==0){//registro n�o ocupado
						if(LOG_LEITURA_ALUNOS)printf("\n   Registrando novo aluno\n      %s", alunos[reg].rg);
						while(linha[x+y]!=SEPARADOR){
							alunos[reg].rg[y]=linha[x+y];
							y++;
						}
						if(LOG_LEITURA_ALUNOS)printf("\n     %s", alunos[reg].nome);
						z=0;
						while(z<QUANTDIG_INST){//grava o nome da institui��o;
							alunos[reg].instituicao[x]=instituicao[x];
							z++;
						}
						z=0;
						while(tempnome[z]!=0){//grava o nome do aluno
							alunos[reg].nome[z]=tempnome[z];
							z++;
						}
					}
					else{//continua comparando com registros existentes
						if(alunos[reg].rg[y]==linha[x+y]) y++;
						else{
							y=0;
							reg++;
						}
					}
				}//QUANDO SAI DAQUI, REGISTRO DO RG EM QUEST�O FEITO, com nome preenchido tamb�m
				x=x+y;
				if(linha[x]!=SEPARADOR){
					printf("\nERRO ENS01\n          LINHA: %s\n          LINHA[%d]: %c\n          Y=%d", linha, x,linha[x],y);
					sair();
				}//se passar daqui signific que ainda est� tudo certo. (como base a contagem dos marcadores, e d�gitos do RG)
				x++;//deixa o cursor em cima da IDA
				if(LOG_LEITURA_ALUNOS)printf("\n   Gravando IDA");
				if(linha[x]==32||linha[x]==88) alunos[reg].transporte[dia][turno*2]=linha[x];
				else{//valor n�o � espa�o nem X
					printf("\nERRO EDI01");
					sair();
				}
				if(LOG_LEITURA_ALUNOS)printf(" [OK] transporte{%d}{%d} = %c", dia, turno, alunos[reg].transporte[dia][turno*2]);
				x++;
				if(linha[x]!=SEPARADOR){
					printf("\nERRO ENS02");
					sair();
				}
				x++;
				if(LOG_LEITURA_ALUNOS)printf("\n   Gravando VOLTA");
				if(linha[x]==32||linha[x]==88) alunos[reg].transporte[dia][1+turno*2]=linha[x];
				else{//valor n�o � espa�o nem X
					printf("\nERRO EDI01");
					sair();
				}
				if(LOG_LEITURA_ALUNOS)printf(" [OK] transporte{%d}{%d} = %c", dia, turno, alunos[reg].transporte[dia][1+turno*2]);
				break;
			default:
				printf("\n\nERRO PFM01");
				sair();
				break;
		}
	}//finalizou a leitura do documento
	//iniciando escrita das informa��es
	if(LOG_GRAVACAO)printf("\nLeitura do arquivo finalizada\n\nCriando arquivo de resultado");
	fclose(fluxo);
	if((fluxo=fopen("Resultado.txt","wt"))==NULL){
		printf("\n\nFalha ao criar o arquivo de resultado...");
		sair();
	}
	if(LOG_GRAVACAO)printf(" [OK]\nIniciando escrita...");
	reg=0;
	while(alunos[reg].rg[0]!=0){
		fprintf(fluxo,"\n%s%c%s%c%s%c", alunos[reg].instituicao, SEPARADOR, alunos[reg].nome, SEPARADOR, alunos[reg].rg, SEPARADOR);//NOME SEPARADOR RG SEPARADOR
		if(LOG_GRAVACAO)printf("\n%s%c%s%c", alunos[reg].nome, SEPARADOR, alunos[reg].rg, SEPARADOR);
		dia=0;
		while(dia<6){
			turno=0;
			while(turno<8){
				fprintf(fluxo,"%c%c", alunos[reg].transporte[dia][turno],SEPARADOR);
				if(LOG_GRAVACAO)printf("%c%c",alunos[reg].transporte[dia][turno],SEPARADOR);
				turno++;
			}
			dia++;
		}
		reg++;
	}
	fclose(fluxo);
	printf("\n\nTarefa concluida com sucesso!\n\nAbrindo arquivo...");
	system("start Resultado.txt");
	printf(" [OK]");
	sair();
}


//By WilliamPilger >> github.com/williampilger

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <windows.h>
//#include <time.h>
#include <conio2.h>

#define LOG 1 //ativa ou n√£o o LOG em arquivo
#define CORSEL 240 //cor do texto quando est· selecionado
#define CORPADRAO 15 //cor do texto padr„o
#define CORSIM 180 //cor do texto ATIVADO
#define CORNAO 128 //cor do texto DESATIVADO
#define CORERRO 192 //COR ERRO PARADA
#define CORDESTAQUE 240//Cor para destacar um texto
#define NUMINITERARIOS 20 //m√°ximo de itiner√°rios poss√≠veis
#define NUMALUNOS 600 //maximo de alunos cadastrados
#define NUMINSTITUICOES 30 //m√°ximo de instituicoes cadastradas
#define DIGNOME 50 //max caracteres do nome do aluno
#define DIGCPF 11 //max caracteres do CPF permitidos
#define DIGRG 15 //max digitos no RG
#define DIGEMAIL 30 //max caracteres do email do aluno
#define DIGFONE 15 //max digitos do telefone do aluno
#define DIGITINERARIO 50 //max digitos nome do itiner√°rio
#define DIGINSTITUICAO 150 //max digitos do nome da instituicao
#define DIGPESQUISA 150 //m·ximo de digitos de uma pesquisa
#define DIGDADO 150 //m·ximo de dÌgitos do dados nos quais ser· feita uma pesquisa (ESTE VALOR PRECISA SER MAIOR QUE O MENOR TAMANHO DE DADO)
#define NUMINST 50 //numero maximo de instituicoes por itiner√°rio
#define NUMPASS 200 //maximo de alunos por itiner√°rios
#define NUMRESULTPESQUISA 20 //m·ximo de resultados por pesquisa
#define MINDIGPESQUISA 3 //mÌnimo de digitos (CONTANDO O ZERO) que precisam ser escritos para iniciar-se a pesquisa autom·tica (na funÁ„o pesquisa cadastro)

struct cadastro_aluno{
    char nome[DIGNOME];
    char cpf[DIGCPF];
    char rg[DIGRG];
    char email[DIGEMAIL];
    char fone[DIGFONE];
    //int horario[7][8];//armazena 1 para cada horario MARCADO do aluno (somente para contagem de viagens/dias do indiv√≠duo. o calculo de passagens e ocupa√ß√£o √© baseado nos dados de viagem na estrutura do transporte)
};
struct cadastro_viagensalunos{
    int aluno;//armazena o INDICE do cadastro do aluno
    int viagens[7][8]; //7 dias da semana por 8 horarios poss√≠veis
};
struct cadastro_instituicao{
    char nome[DIGINSTITUICAO];
};
struct cadastro_itinerario{
    char nome[DIGITINERARIO]; //nome do itiner√°rio
    int instituicao[NUMINST]; //armazena o INDICE dos cadastros das instituicoes
    int dia; //armazena o dia da semana (0 domingo, 1 segunda, ...)
    int horario;// armazena o horario deste itiner√°rio (0 manha, 1 tarde, 2 verpertino, 3 noite)
    struct cadastro_viagensalunos passagens[NUMPASS];
};

struct cadastro_aluno alunos[NUMALUNOS]={0}; //estrutura com os cadasros dos alunos
struct cadastro_itinerario itinerarios[NUMINITERARIOS]={0}; //estrutura com os itiner√°rios
struct cadastro_instituicao instituicoes[NUMINSTITUICOES]={0}; //estrutura com as institui√ß√µes

FILE *logfile;
#define LOGLIM 300 ///n√∫mero max de caracteres do LOG
void pontolog(const char text[LOGLIM]){
    fprintf(logfile,"\n%s", text);
}

void sair(int tipo){
    clrscr();
    switch(tipo){
        case -1:
            if(LOG) pontolog("ERRO INTERNO. JUSTIFICATIVA -1. FINALIZANDO.");
            printf("\n\nOOOPS, ESTA FUNCAO AINDA NAO ESTA DISPONIVEL NESTA BUILD, MAS ESTAMOS TRABALHANDO NISSO.\n ENTRE EM CONTATO COM O DESENVOLVEDOR PARA MAIS INFORMACOES.\n O PROGRAMA SERA FINALIZADO");
            break;
        case 0://ERRO INTERNO LIGADO A OPCOES/SWITCH(OPCAO) OU SEMELHANTES
            if(LOG) pontolog("ERRO INTERNO. OPCAO INESPERADA. JUSTIFICATIVA 0. FINALIZANDO.");
            printf("\n\nDESCULPE, TIVEMOS UM PROBLEMA AQUI DO NOSSO LADO.\n ENTRE EM CONTATO COM O DESENVOLVEDOR PARA MAIS INFORMACOES.\n O PROGRAMA SERA FINALIZADO");
            break;
        case 1://FALHA DE PERMISS√ÉO DE LEIURA/ESCRITA
            if(LOG) pontolog("ERRO NO ACESSO A ARQUIVOS. JUSTIFICATIVA 1. FINALIZANDO.");
            printf("\n\nDESCULPE, TIVEMOS UM PROBLEMA PARA MANIPULAR ARQUIVOS. VERIFIQUE SE VOCE POSSUI PERMISSAO PARA ESCREVER NESTE DIRETORIO.\n O PROGRAMA SERA FINALIZADO");
            break;
        case 2://usuario n√£o permitiu continuidade
            if(LOG) pontolog("PROCESSO NAO PERMITIDO POR USUARIO. JUSTIFICATIVA 2. FINALIZANDO.");
            printf("\n\nDESCULPE, MAS NAO PODEREMOS SEGUIR SEM ESTA PERMISSAO.\n O PROGRAMA SERA FINALIZADO");
            break;
        case 3://ERRO INTERNO REFERENTE A INCONSIST NCIA NO VALOR DE VARI¡VEIS
            if(LOG) pontolog("ERRO INTERNO. VARIAVEL INCONSISTENTE. JUSTIFICATIVA 3. FINALIZANDO.");
            printf("\n\nDESCULPE, TIVEMOS UM PROBLEMA AQUI DO NOSSO LADO.\n ENTRE EM CONTATO COM O DESENVOLVEDOR PARA MAIS INFORMACOES.\n O PROGRAMA SERA FINALIZADO");
            break;
        case 4:
        	if(LOG) pontolog("ERRO INTERNO. ERRO DE COMPILACAO. VERIFIQUE OS #DEFINE'S. JUSTIFICATIVA 4. FINALIZANDO.");
        	printf("\n\nDESCULPE, TIVEMOS UM PROBLEMA AQUI DO NOSSO LADO.\n ENTRE EM CONTATO COM O DESENVOLVEDOR PARA MAIS INFORMACOES.\n O PROGRAMA SERA FINALIZADO");
            break;
        default:
            if(LOG) pontolog("ERRO DESCONHECIDO. FINALIZANDO.");
            printf("\n\nERRO INESPERADO.\n O PROGRAMA SERA FINALIZADO");
            break;
    }
    printf("\n\nPressione qualquer tecla para fechar esta janela");
    getch();
    exit(0);
}

int busca_em_string(char *pesquisa, int digpes, char *dado, int digdado){
	//recebe:
	//ponteiro CHAR "pesquisa" - ContÈm a pesquisa
	//int "digpes"             - n˙mero exato de dÌgitos da pesquisa
	//ponteiro CHAR "digdado"  - contÈm a string onde ser· buscado o conte˙do de "pesquisa"
	//int digdado              - n˙mero de dÌgitos m·ximo do dado no qual ser· pesquisado
	//retorna: 0 - busca negativa     // 1 - busca positiva
	if(LOG) pontolog(" INICIANDO FUNCAO DE PESQUISA EM STRING.");
	int z=0, w;
	char *p, *d;
	while(z<=(digdado-digpes)){
		p = pesquisa;
		d = dado;
		w=0;
		while(w<z){//while para avanÁar a pesquisa (deslocar a pesquisa na string onde est· se procurando
			d++;
			w++;
		}
		w=0;
		while(w<digdado&&w<digpes){
			if(pesquisa==dado){
				dado++;
				pesquisa++;
				w++;
			}
			else break;//pesquisa nao corresponde
			if(w==digpes-1){//encontrou o final da pesquisa, ou seja, ultimo dÌgito ainda era coincidente
				if(LOG) pontolog("  RESULTADO POSITIVO OBTIDO. RETORNANDO.");
				return 1;
			}
		}
		z++; //se ainda existe "espaÁo" deslocar a pesquisa ela ser· deslocada. se n„o o final da pesquisa chegou.
	}
	if(LOG) pontolog("  NENHUM RESULDADO ENCONTRADO. RETORNANDO.");
	return 0; //n]„o teve resultado positivo
}

void pesquisa_alunos(void){
	if(LOG) pontolog(" INICIANDO PESQUISA DE CADASTROS DE ALUNOS");
	char pesquisa[DIGPESQUISA]={0};
	unsigned char dig;
	int nresult,//n˙mero de resultados da pesquisa
		x=0,//armazena posiÁ„o do cursor na pesquisa
		y,//controle do laÁo externo da pesquisa
		z,//controle do deslocamento da pesquisa (laÁos intermedi·rios)
		w,//controle dos laÁos mais internos da pesquisa
		digitos,//armazena a contagem de digitos da string na qual ser· pesquisado
		opcao,//armazena opcao selecionada pelo usu·rio (resultado de pesquisa selecionado
		result[NUMRESULTPESQUISA]={0},//armazena os Ìndices dos cadastros de aluno que correspondem com a pesquisa 
		pes=0;//seta em 1 quando a pesquisa deve ser atualizada
	while(1){
		if(pesquisa[x]!=0) sair(3);//PRECISA SER ZERO SEMPRE NESTE PONTO
		textcolor(CORPADRAO);
		if(LOG) pontolog("  LIMPA A TELA; REESCREVE CONTEUDO ATUALIZADO DA PESQUISA; AGUARDA PROXIMO DIGITO");
		clrscr();
		printf("Sua pesquisa: %s\n\n", pesquisa);
		dig=getch();
		switch(dig){
			case 27://esc
				if(LOG) pontolog("  ESC PRESSIONADO; FINALIZADA A FUNCAO.");
				return;
				break;
			case 8: //backspace
				if(LOG) pontolog("  USUARIO PRESSIONA BACKSPACE");
				if(x>0) x--;
				pesquisa[x]=0;
				opcao=0;
				pes=1;
				break;
			case 224://pressiona tecla de seta
				if(LOG) pontolog("  USUARIO PRESSIONA SETA");
				dig=getch();
				switch(dig){
					case 80://para baixo
						if(LOG) pontolog("   PARA BAIXO");
						if(opcao<nresult-1) opcao++;//avanÁa opcao
						break;
					case 72://para cima
						if(LOG) pontolog("   PARA CIMA");
						if(opcao>0) opcao--;
						break;
				}
				break;
			default:
				if((dig>=48&&dig<=57)||(dig>=97&&dig<=122)||(dig>=65&&dig<=90)){//se for n˙mero, ou letra (maiuscula e minuscula) È aceito.
					if(LOG) pontolog("  USUARIO PRESSIONA TECLA VALIDA");
					pesquisa[x]=dig;
					if(x<DIGPESQUISA) x++;
					pes=1;
				}else if(LOG) pontolog("  USUARIO PRESSIONA TECLA INVALIDA");
				opcao=0;
				break;
		}
		//printf("tru\n%s\n%d\n%d", alunos[0].nome, y, alunos[0].cpf[0]); getch();
		//inicia-se a pesquisa
		if(pes&&(x>MINDIGPESQUISA)){//se a pessoa j· digitou mais que o mÌnimo de dÌgitos
			if(LOG) pontolog("  INICIANDO ATUALIZACAO DA PESQUISA");
			y=0;
			nresult=0;
			while(y<NUMALUNOS){
				//printf("tru\n%s\n%d\n%d", alunos[0].nome, y, alunos[0].cpf[0]); getch();
				if(alunos[y].cpf[0]!=0){//o cadastro È v·lido
					for(digitos=0; digitos<DIGNOME&&alunos[y].nome[digitos]!=0; digitos++){}//fica no loop atÈ encontrar o final do campo
					if(busca_em_string(&pesquisa[0], digitos, &alunos[y].nome[0], DIGNOME)){//se o nome concide
						printf("TRAA");getch();
						result[nresult]=y+1;//salva o indice do cadastro do aluno que foi encontrado
						nresult++;//soma o n˙mero de resultados
					}/*else if(){//fazer outra pesquisa se o nome n„o coincide
						#####
					}*/
					
				}
				else if(LOG) pontolog("   CADASTRO INVALIDO");
				y++;//indice do cadastro de alunos
			}
		}else if(LOG) pontolog("  PESQUISA NAO ATUALIZADA");
		//inicia escrita dos resultados
		if(LOG) pontolog("  INICIANDO ESCRITA DOS RESULTADOS");
		for(y=0;y<NUMRESULTPESQUISA&&result[y]!=0;y++){
			printf("\n%s", alunos[result[y]-1].nome);//ta certo a variavel, mas isso n„o funciona
			//getch();
		}
		printf("\n\nTESTE-FORA");
	}
}

void editcadas_alunos(void){
	printf("tru\n%s\n%d", alunos[0].nome, alunos[0].cpf[0]); getch();
	if(LOG) pontolog(" INICIADO MENU DE EDICAO DE CADASTROS DE ALUNOS");
	unsigned char dig=0;
	while(1){
		clrscr();
	    printf("\n\n  MANIPULAR CADASTROS DE ALUNOS\n\n");
	    printf("\n0 - Pesquisar nos cadastros");
	    printf("\n1 - Incluir novo cadastro de aluno");
	    printf("\n2 - Exportar dados de alunos");
	    printf("\n\nESC - Voltar ao menu anterior");
	    if(LOG) pontolog("  AGUARDANDO OPCAO DO USUARIO");
	    while(dig<'0'||dig>'3'){
	        dig=getch();
	        if(dig==27){
	        	if(LOG) pontolog("  USUARIO PRESSIONOU ESC");
	        	return;
	        }
	    }
	    switch(dig){
	    	case '0'://Pesquisar cadastros
	    		if(LOG) pontolog("  INICIANDO PESQUISA DE CADASTROS DE ALUNOS");
	    		pesquisa_alunos();
				if(LOG) pontolog("  FINALIZADA A PESQUISA DE CADASTROS");
	    		break;
	    	case '1'://Novo Cadastro
	    		sair(-1);
	    		break;
	    	case '2'://Exportar dados de alunos
	    		sair(-1);
	    		break;
	    	default:
	    		sair(0);
	    		break;
	    }
	}
}

int editcadas_instituicoes(void){

}

int editcadas_itinerarios(void){

}

int menu_principal(void){
    if(LOG) pontolog(" MENU PRINCIPAL INICIADO");
    int opcao;
    unsigned char dig;
    clrscr();
    printf("\n\n  MENU PRINCIPAL\n\n");
    printf("\n0 - Manipular cadastros de alunos");
    printf("\n1 - Manipular cadastros de instituicoes");
    printf("\n2 - Manipular cadastros dos itinerarios");
    printf("\n3 - Calcular e exportar dados");
    if(LOG) pontolog("  AGUARDANDO OPCAO DO USUARIO");
    while(dig<'0'||dig>'3'){
        dig=getch();
    }
    switch(dig){
        case '0':
            if(LOG) pontolog("  RETORNADA OPCAO 0");
            return 0;
            break;
        case '1':
            if(LOG) pontolog("  RETORNADA OPCAO 1");
            return 1;
            break;
        case '2':
            if(LOG) pontolog("  RETORNADA OPCAO 2");
            return 2;
            break;
        case '3':
            if(LOG) pontolog("  RETORNADA OPCAO 3");
            return 3;
            break;
        default:
            sair(0);
            break;
    }
}


int main(void){
	if(DIGEMAIL>DIGDADO||DIGNOME>DIGDADO||DIGCPF>DIGDADO||DIGRG>DIGDADO||DIGITINERARIO>DIGDADO||DIGINSTITUICAO>DIGDADO||DIGPESQUISA>DIGDADO) sair(8);
	logfile = fopen("log.txt", "wt");
    textcolor(CORPADRAO);
    clrscr();
    printf("\n\n                     Bem vindo ao TransSoft!");
    printf("\n\n Software para calculo de valor de passagem, numero de carros, e muito mais...");
    textcolor(CORDESTAQUE);
    printf("\n\n            Pressione qualquer tecla para iniciarmos...            ");
    textcolor(CORPADRAO);
    printf("\n\n\n\n\n Desenvolvimento: William Pilger | +55 51 997 843 909 | pilger.will@gmail.com");
    unsigned char dig;
    dig=getch();
    if(LOG) pontolog(" BUSCANDO POR BANCO DE DADOS");
    FILE* bancodados;
    if((bancodados=fopen("banco_de_dados.bddae", "rt"))==NULL){
        if(LOG) pontolog("  NENHUM BANCO EXISTENTE");
        clrscr();
        printf("\n Nao foi encontrado um banco de dados, deseja criar um novo? (s/n)");
        dig = 0;
        if(LOG) pontolog("   AGUARDANDO RESPOSTA DO USUARIO PARA 'deseja criar novo?'");
        while(dig!='s'&&dig!='n'&&dig!='S'&&dig!='N'){
            dig=getch();
        }
        if(dig=='s'||dig=='S'){
            if((bancodados=fopen("banco_de_dados.bddae", "wt"))==NULL){
                if(LOG) pontolog("   RESPOSTA DO USUARIO: SIM");
                sair(1);
            }
            if(LOG) pontolog("  NOVO BANCO DE DADOS CRIADO");
        }
        else {
            if(LOG) pontolog("   RESPOSTA DO USUARIO: NAO");
            sair(2);
        }
    }else{ //nesse caso, existe banco de dados, ent√£o dever√° ser lido
        if(LOG) pontolog(" LENDO BANCO DE DADOS");
        //####DEVE SER FEITA A LEITURA DO BANCO DE DADOS 
    }
    if(LOG) pontolog("CHAMANDO MENU PRINCIPAL");
    int opcao;
    while(1){
        opcao=menu_principal();
        switch(opcao){
            case 0:
                if(LOG) pontolog("INICIANDO EDICAO DE CADASTROS DOS ALUNOS");
                alunos[0].nome[0]='W';
                alunos[0].nome[1]='i';
                alunos[0].nome[2]='l';
                alunos[0].nome[3]='l';
                alunos[0].cpf[0]=1;
                printf("tru\n%s\n%d esperando", alunos[0].nome, alunos[0].cpf[0]); getch();
				editcadas_alunos();
                if(LOG) pontolog("FIM EDICAO DE CADASTROS DOS ALUNOS");
                break;
            case 1:
                if(LOG) pontolog("INICIANDO EDICAO DE CADASTROS DAS INSTITUICOES");
                editcadas_instituicoes();
                if(LOG) pontolog("FIM EDICAO DE CADASTROS DAS INSTITUICOES");
                break;
            case 2:
                if(LOG) pontolog("INICIANDO EDICAO DE CADASTROS DOS ITINERARIOS");
                editcadas_itinerarios();
                if(LOG) pontolog("FIM EDICAO DE CADASTROS DOS ITINERARIOS");
                break;
            case 3:
                sair(-1);//cod n√£o desenvolvido
                break;
            default:
                sair(0);
                break;
        }
    }
}

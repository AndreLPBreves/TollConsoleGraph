#include "stdio.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h> 

const char* ANSI_COLOR_YELLOW = "\x1b[33m";
const char* ANSI_COLOR_GREEN = "\x1b[32m";
const char* ANSI_COLOR_CYAN = "\x1b[36m";
const char* ANSI_COLOR_RESET = "\x1b[0m";

//Usei este método porque usar apenas const não é suficiente para poder usar a variável na hora de definir o tamanho de um array
enum constants
{
	TOTAL_TOLL_RECORDS = 1000,
	MAX_BAR_LENGTH = 39
};

//Criei este enum para facilitar a leitura dos dias da semana no código
typedef enum WeekDay
{
	Sun = 0,
	Mon,
	Tue,
	Wed,
	Thu,
	Fri,
	Sat
} WeekDay;

//Este vetor tem os nomes que serão exibidos no output e são uma relação direta com o enum, podendo traduzir de um para o outro
const char* weekDayNames[] =
{
	"Domingo",
	"Segunda",
	"Terca",
	"Quarta",
	"Quinta",
	"Sexta",
	"Sabado"
};

typedef struct TollData
{
	WeekDay Day;
	int CarCount;
} TollData;

/*
Função responsável por gerar os gráficos
A função recebe o dia inicial, a quantidade de dias a partir daí, um ponteiro para o array de TollData e o numero total de dados dentro do array.
*/
static void graphTollDataSlice(int ini, int dayCount, TollData* tollRecords, int totalTollRecords)
{
	int totalCars = 0;
	int mean = 0;
	TollData* lowestDay = NULL;
	TollData* highestDay = NULL;
	
	/*
	Essa parte é responsável por impedir que passemos um intervalo que saia do array
	*/
	if (ini > totalTollRecords) ini = totalTollRecords-1;
	if (ini + dayCount > totalTollRecords) dayCount = totalTollRecords - ini;

	/*
	Aqui é onde o calculo para encontrar o total de carros no período, o dia com menos tráfego e o dia com mais tráfego
	*/
	for (int i = ini; i < ini + dayCount; i++)
	{
		totalCars += tollRecords[i].CarCount;
		if (lowestDay == NULL) lowestDay = &tollRecords[i];
		else if (lowestDay->CarCount > tollRecords[i].CarCount) lowestDay = &tollRecords[i];

		if (highestDay == NULL) highestDay = &tollRecords[i];
		else if (highestDay->CarCount < tollRecords[i].CarCount) highestDay = &tollRecords[i];
	}

	/*
	Fazemos essa checagem para garantir que não prossigamos com ponteiros nulos
	*/
	if (lowestDay == NULL || highestDay == NULL) return;

	/*
	Aqui fazemos o cálculo da média de carros no período e o tamanho da barra para que se mantenha proporcional ao tráfego quando imprimir no gráfico
	*/
	mean = totalCars / dayCount;
	int scale = highestDay->CarCount / MAX_BAR_LENGTH;

	printf("\n%s===================================================================%s\n", ANSI_COLOR_CYAN, ANSI_COLOR_RESET);
	printf("                  RELATORIO DE TRAFEGO | DIA %d a DIA %d", ini + 1, ini + dayCount);
	printf("\n%s===================================================================%s\n", ANSI_COLOR_CYAN, ANSI_COLOR_RESET);
	printf("Grafico de Barras (Escala: 1 barra ~~ %d carros)\n\n", scale);

	for (int i = ini; i < ini + dayCount; i++)
	{
		int carCount = tollRecords[i].CarCount;
		int barLength = carCount / scale;

		const char* color = ANSI_COLOR_RESET;
		if (&tollRecords[i] == highestDay) color = ANSI_COLOR_YELLOW;
		else if (&tollRecords[i] == lowestDay) color = ANSI_COLOR_GREEN;
		else if (carCount > mean) color = ANSI_COLOR_CYAN;

		printf("%s %-7s (Dia %4d): |",
			color,
			weekDayNames[tollRecords[i].Day],
			i + 1);

		for (int j = 0; j < barLength; j++) {
			printf("%s*%s", color, ANSI_COLOR_RESET);
		}

		printf("%*s%s%6d%s\n",
			MAX_BAR_LENGTH - barLength, "",
			color,
			carCount,
			ANSI_COLOR_RESET);
	}

	printf("\n%s---------------------------------%s\n", ANSI_COLOR_CYAN, ANSI_COLOR_RESET);
	printf("|          L E G E N D A        |\n");
	printf("%s---------------------------------%s\n", ANSI_COLOR_CYAN, ANSI_COLOR_RESET);
	printf("| %s* PICO (Maior Movimento)%s      |\n", ANSI_COLOR_YELLOW, ANSI_COLOR_RESET);
	printf("| %s* BAIXA (Menor Movimento)%s     |\n", ANSI_COLOR_GREEN, ANSI_COLOR_RESET);
	printf("| %s* Acima da Media%s              |\n", ANSI_COLOR_CYAN, ANSI_COLOR_RESET);
	printf("%s---------------------------------%s\n", ANSI_COLOR_CYAN, ANSI_COLOR_RESET);
	printf("\n%s===================================================================%s\n", ANSI_COLOR_CYAN, ANSI_COLOR_RESET);
	printf("RESUMO ESTATISTICO:\n");
	printf("  Periodo Analisado: %d dias\n", dayCount);
	printf("  Total de Carros:   %ld\n", totalCars);
	printf("  Media Diaria:      %s%d carros%s\n", ANSI_COLOR_CYAN, mean, ANSI_COLOR_RESET);

	printf("\n  Dia de MAIOR Movimento:\n");
	printf("    -> %s%s (Dia %d) com %d carros%s\n",
		ANSI_COLOR_YELLOW,
		weekDayNames[highestDay->Day],
		(int)(highestDay - tollRecords) + 1,
		highestDay->CarCount,
		ANSI_COLOR_RESET);

	printf("  Dia de MENOR Movimento:\n");
	printf("    -> %s%s (Dia %d) com %d carros%s\n",
		ANSI_COLOR_GREEN,
		weekDayNames[lowestDay->Day],
		(int)(lowestDay - tollRecords) + 1,
		lowestDay->CarCount,
		ANSI_COLOR_RESET);

	printf("\n%s===================================================================%s\n", ANSI_COLOR_CYAN, ANSI_COLOR_RESET);
}

static void clearScreen(void)
{
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif
}

static bool waitForInput() {
	printf("\n-------------------------------------------------------------\n");
	printf("Pressione QUALQUER TECLA para nova consulta ou ESC para SAIR.");
	printf("\n-------------------------------------------------------------\n");

	int c;
	while ((c = getchar()) != '\n' && c != EOF) {}

	int key;
#ifdef _WIN32
	key = _getch();
#else
	key = getchar();
#endif

	if (key == 27) {
		return false;
	}

	return true;
}

int main(void)
{
	bool isRunning = true;
	int startDay = 0;
	int numDays = 0;

	srand((unsigned int)time(NULL));

	TollData tollRecords[TOTAL_TOLL_RECORDS] = {0};

	for (int i = 0; i < TOTAL_TOLL_RECORDS; i++)
	{
		WeekDay currentDay = i % 7;

		//Aqui eu dandomizei o trafego. Um numero aleatório de 0 a 30000 + 30000, então de 30000 a 60000. Depois eu multiplico por um fator entre 0.9 e 1.1;
		float trafic = ((float)((rand() % 30001) + 30000)) * ((float)((rand() % 21) + 90) / 100.0f);

		//Para simular picos de movimento nos finais de semana, peguei o resultado do tráfego e multipliquei por um fator entre 1.1 e 1.3
		if (currentDay == Sat || currentDay == Sun) trafic = trafic * ((float)((rand() % 21) + 110) / 100.0f);

		tollRecords[i].Day = currentDay;
		tollRecords[i].CarCount = (int)trafic;
	}

	printf("Bem-vindo ao sistema de monitoramento de fluxo!\n");

	while (isRunning)
	{
		clearScreen();

		printf("--- Sistema de Analise de Pedagio ---\n\n");
		printf("Para ver o grafico de um intervalo, informe o dia inicial (1 a %d) e uma quantidade de dias.\n", TOTAL_TOLL_RECORDS);
		printf("Ex.: 100, 20\n");
		printf("Dia inicial (1 = primeiro registro): ");

		scanf_s("%d", &startDay);

		if (startDay > 0) startDay--;
		else startDay = 0;

		printf("Quantidade de dias: ");
		scanf_s("%d", &numDays);
		if (numDays < 1) numDays = 1;

		graphTollDataSlice(startDay, numDays, tollRecords, TOTAL_TOLL_RECORDS);
		isRunning = waitForInput();
	}

	clearScreen();
	printf("Programa finalizado. Ate mais!\n");

	return 0;
}
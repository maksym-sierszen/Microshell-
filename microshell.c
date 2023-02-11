
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <time.h>

const char* historyFilename = "history.txt";
char historyPath[256];
extern const char *__progname;
// getHistoryPath - zwraca sciezke absolutna do pliku history.txt, implementacja dla linuxa, po zawolaniu trzeba zwolnic pamiec
char *getHistoryPath(){
    char *buf = malloc(256);
    readlink("/proc/self/exe", buf, (256-strlen(historyFilename)));
    int pos = strlen(buf)-strlen(__progname);
    buf[pos] = '\0';
    strcat(buf, historyFilename);
    return buf;
}

// displayPath - wyswietla znak zachety w postaci [{path}] $, gdzie {path} jest sciezka do biezacego katalogu roboczego
char displayPath()
{
    char path[1024];
    char *currentUser = getlogin();	// getlogin zwraca wskaznik do nazwy usera
    char *cwd = getcwd(path, 1024);	// getcwd zwraca wskaznik do obecnej sciezki (current working directory)
    if(cwd!=NULL)
    {
        printf("[\033[34m%s:\033[0m\033[36m%s\033[0m]\n$ ", currentUser, cwd);	// print [user:cwd] $	//kolory: dark blue, cyan
    }
    return *cwd; // Zwraca cwd
}

// addHistory - dodaj zapis do pliku history.txt
void addHistory(char *input)
{
	FILE *file;
	file = fopen(historyPath, "a");
	fputs(input, file);
	fclose(file);
}


// getInput - Zbierz input usera 
char *getInput(void)
{
    char *input = NULL;	// Zmienna inputowa
    input = malloc(1024 * sizeof(char));	// Przypisz pamiec dla tej zmiennej
    if(input == NULL)
	{
		perror("error occured: malloc"); // Jesli nie uda sie przypisac pamieci do input to wyrzuc blad
		exit(1);
	}

    fgets(input, 1024, stdin);	// Odczytaj input usera  
	addHistory(input); // Dodaj wpis do historii
	return input; // Funkcja zwraca wskaznik na czytany ciag znakow
}


// splitInput - Rozdziel input usera na tokeny 
char **splitInput(char *input)
{
	char *separators = " \n\t";	// Rozdzielanie tokenow wg spacji i tabulatora
	int counter = 0;	// licznik

	char **tokens = malloc(256 * sizeof(char*));	// Alokuje pamiec dla tablicy wskaznikow na ciagi znakow
	char *token = strtok(input, separators);	// Wydziel token


	while(token != NULL) // strtok zwroci NULL gdy skonczy znajdowac tokeny w inpucie
	{
		tokens[counter] = token;
		counter++;
		token = strtok(NULL, separators); // Funkcja strtok () analizuje ciąg znaków w sekwencji tokenów.
										  // Przy pierwszym wywołaniu strtok () łańcuch do przeanalizowania powinien być określony w str .
										  // W każdym kolejnym wywołaniu, które powinno analizować ten sam ciąg, str powinno mieć wartość NULL.
	}

	tokens[counter] = NULL;	// Tablica wskaźników MUSI być zakończona wskaźnikiem NULL aby execvp moglo wykonac swoja prace
	return tokens; // Zwraca wskaznik tokens
}


// cdFunc - zmienia katalog roboczy
void cdFunc(char **args)
{
	char tildePath[256]; 
	if (args[1] == NULL) // Jesli nie podano sciezki do ktorej przejsc
	{
		printf("error occured: cd: no path specified\n");
	}
	else
	{
		if (args[1][0] == '~') // Sprawdza czy pierwszy znak w podanym ciagu znakow to tylda
		{
			strcpy(tildePath, getenv("HOME"));
			strcat(tildePath, &args[1][1]);

			if(chdir(tildePath) != 0) // Funkcja chdir powinna zwracac 0 w przypadku sukcesu
			{
				perror("error occurred: cd");
			}
		}
		else if (chdir(args[1]) != 0) // Funkcja chdir powinna zwracac 0 w przypadku sukcesu
		{
				perror("error occurred: cd");
		}	
	}
}



// helpFunc - wyswietla informacje o powloce
void helpFunc()
{
	printf("\033[1m\033[4m[Uniwersytet im. Adama Mickiewicza w Poznaniu]\033[0m\033[0m\nSOP - Microshell\nAutor: Maksym Sierszen\n\ncd\nhelp\nexit\nhistory [-c]\ndate\n");
				//bold //underline
}

// exitFunc - konczy dzialanie programu
void exitFunc()
{
	exit(0);
}

// historyFunc - wypisz historie komend usera
void historyFunc(char **args)
{
	FILE *file;
	int counter = 1; // Licznik wierszy w pliku
	char line[256]; // Zmienna danego wiersza

	if(args[1] != NULL) // Jesli jest flaga
	{
		if(args[1][1] == 'c') // Sprawdz czy to -c (wyczysc historie)
		{
			file = fopen(historyPath, "w"); // Open z paramterem "w" wystarczy do wyczyszczenia pliku
			if(file == NULL)	// Jesli blad to komunikat
			{
				perror("error occured: history");
				return;
			}
		}
	}
	else 
	{
		file = fopen(historyPath, "r");	// W przypadku braku flagi otworz z "r" do odczytywania
		if(file == NULL)	// Jesli blad to komunikat
		{
			perror("error occured: history");
			return;
		}
		while (fgets(line, sizeof(line), file))	// Pobieraj kazda linijke pliku
		{
			printf("%d %s", counter, line);	// Printuj obecna linijke
			counter ++;	// Inkrementuj licznik
		}
	}
	fclose(file);
}

// dateFunc - wypisz date i godzine
void dateFunc()
{
	time_t current;
	time(&current);
	printf("%s", ctime(&current));

}

// funcExecutor - inicjator funkcji
void funcExecutor(char **args)
{
		if(strcmp(args[0], "cd") == 0) // Wykonaj cd
		{
			cdFunc(args);
			return;	
		} 
		else if(strcmp(args[0], "help") == 0) // Wykonaj help
		{
			helpFunc();
			return; 
		} 
		else if(strcmp(args[0], "exit") == 0) // Wykonaj exit
		{
			exitFunc();
			return; 
		} 
		else if(strcmp(args[0], "history") == 0) // Wykonaj history
		{
			historyFunc(args);
			return; 
		} 
		else if(strcmp(args[0], "date") == 0) // Wykonaj date
		{
			dateFunc();
			return; 
		} 

	pid_t pid = fork();	// Fork tworzy nowy proces - (proces-dziecko ma za zadanie uruchomienie polecenia usera)
	int status;

	if(pid == 0)	// Sukces przy forkowaniu
	{
		execvp(args[0], args);
		perror("error occured: execvp"); // Blad tylko jesli pid == 0 nie bedzie sukcesem
		exit(1);
	}
	else if (pid > 0)// W innym przypadku zaczekaj na zakonczenie procesu potomnego
	{
		do{
			waitpid(pid, &status, WUNTRACED ); // WUNTRACED informuje by czekac na procesy, ktore zostaly zatrzymane oraz ktore sie zakonczyly
			}while(!WIFEXITED(status));	// WIFEXITED sprawdza czy child process sie zakonczyl poprawnie, jesli nie, to wroc w petli i czekaj ... 
	}
	  
}



int main()
{
    char *a = getHistoryPath();
    strcpy(historyPath, a);
    free(a);
	while(1)
	{
		displayPath();
		char *input = getInput();
		char **tokens = splitInput(input);
	
		if(tokens[0] != NULL)
		{
			funcExecutor(tokens);
		}
		free(tokens);
		free(input);
		
	}
    return 0;
}

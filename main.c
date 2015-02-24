#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include <string.h>

#define MAXPAIR     1000 // maximum no of country-city pair
#define MAXNAMELEN  50
#define MAXLINELEN  5000 // maximum length of each line

struct countryCityName{
    char *name;
    char *city;
    char *country;
    int isVisited;
    int isRestrictedCity;
    int isRestrictedCountry;
};

struct path{
    int length;
    struct pathItem *pathHead;
    struct path *next;
};

struct pathItem{
    struct countryCityName *item;
    struct pathItem *next;
};

void solutionA(struct path *head, char *toVisit[]);
int hash(char *s);
void readVisitedTxt(void);
char ***extract(char *path);
void readWantToVisitTxt(void);
void print(struct path *head);
void readRestrictedCitiesTxt(void);
void readRestrictedCountriesTxt(void);
void printPath(struct pathItem *head);
struct countryCityName *newNode(char **pair);
void createPathList(char *line, struct path **pathListRef);
void insert(struct pathItem **headRef, struct countryCityName *item);

char *wantToVisit[MAXPAIR];
struct countryCityName *countryCityArray[MAXPAIR];
int visitedCities[MAXPAIR], restrictedCities[MAXPAIR], restrictedCountries[MAXPAIR];

int setVisitedAll;

int main(void)
{
    FILE *dFile;
    char line[MAXLINELEN];
    struct path *pathList = NULL;

    readVisitedTxt();
    readRestrictedCitiesTxt();
    readRestrictedCountriesTxt();

    readWantToVisitTxt();
    
    dFile = fopen("Dependent.txt", "r");
    while(fgets(line, MAXLINELEN, dFile) != NULL){
        line[strlen(line)-1] = '\0';
        createPathList(line, &pathList);
    }

    //print(pathList);

    solutionA(pathList, wantToVisit);
}

void solutionA(struct path *head, char *toVisit[])
{
    int i;
    struct path *current;

    for(i = 0; toVisit[i] != NULL; ++i){
        current = head;
        while(current != NULL){
            if(strcmp(toVisit[i], current->pathHead->item->name) == 0){
               printPath(current->pathHead);
               printf("\n");
            }
            current = current->next;
        }
        printf("\n");
    }
}


void createPathList(char *line, struct path **pathListRef)
{
    int i, h;
    char ***pairArray;
    struct path *newPathNode;

    newPathNode = (struct path *)malloc(sizeof(struct path));

    pairArray = extract(line);

    for(i = 0; pairArray[i] != NULL; ++i){
        h = hash(pairArray[i][0]);
        if(countryCityArray[h] == NULL)
            countryCityArray[h] = newNode(pairArray[i]);
        insert(&(newPathNode->pathHead), countryCityArray[h]);
    }

    newPathNode->length = i;    // setting length flag
    
    if(*pathListRef == NULL){
        *pathListRef = newPathNode;
        newPathNode->next = NULL;
    }
    else{
        newPathNode->next = *pathListRef;
        *pathListRef = newPathNode;
    }
}

void insert(struct pathItem **headRef, struct countryCityName *item)
{
    struct pathItem *new = (struct pathItem *)malloc(sizeof(struct pathItem));

    new->item = item;
    if(*headRef == NULL){
        *headRef = new;
        new->next = NULL;
    }
    else{
        new->next = *headRef;
        *headRef = new;
    }
}

// extracts each country-city pair from a given path
char ***extract(char *path)
{
    int i;
    char *a, *b, *c;
    char name[MAXNAMELEN];
    char city[MAXNAMELEN];
    char country[MAXNAMELEN];

    char ***pairArray = (char ***)calloc(MAXPAIR, sizeof(char **));

    for(i = 0; *path != '\0'; i++){
        a = name;
        b = country;
        c = city;

        while(*path != '\0' && (*a++ = *path) != ':')   // extracting country name
            *b++ = *path++;
        path++;

        while(*path != '\0' && *path != '/'){           // extracting city name
            *a++ = *path;
            *c++ = *path++;
        }
        if(*path != '\0')
            path++;

        *a = '\0';
        *b = '\0';
        *c = '\0';

        pairArray[i] = (char **)calloc(3, sizeof(char *));
        pairArray[i][0] = strdup(name);
        pairArray[i][1] = strdup(country);
        pairArray[i][2] = strdup(city);
    }
    pairArray[i] = NULL;

    return pairArray;
}

// newNode creates a new node of type countryCityName
struct countryCityName *newNode(char **pair)
{
    struct countryCityName *new = (struct countryCityName *) malloc(sizeof(struct countryCityName));

    new->name       = pair[0];
    new->country    = pair[1];
    new->city       = pair[2];

    if (visitedCities[hash(pair[0])])
        new->isVisited = true;

    if (restrictedCountries[hash(pair[1])])
        new->isRestrictedCountry = true;

    if (restrictedCities[hash(pair[0])])
        new->isRestrictedCity = true;

    return new;
}

void readWantToVisitTxt(void)
{
    int i;
    char buf[MAXNAMELEN];
    FILE *fp = fopen("WantToVisit.txt", "r");

    i = 0;
    while (fgets(buf, MAXNAMELEN, fp) != NULL) {
            buf[strlen(buf)-1] = '\0';
            wantToVisit[i++] = strdup(buf);
    }
    wantToVisit[i] = NULL;
}

void readVisitedTxt(void)
{
    char buf[MAXNAMELEN];
    FILE *fp = fopen("Visited.txt", "r");

    while (fgets(buf, MAXNAMELEN, fp) != NULL) {
            buf[strlen(buf)-1] = '\0';
            visitedCities[hash(buf)] = true;
    }
}

void readRestrictedCitiesTxt(void)
{
    char buf[MAXNAMELEN];
    FILE *fp = fopen("RestrictedCities.txt", "r");

    while (fgets(buf, MAXNAMELEN, fp) != NULL) {
            buf[strlen(buf)-1] = '\0';
            restrictedCities[hash(buf)] = true;
    }
}

void readRestrictedCountriesTxt(void)
{
    char buf[MAXNAMELEN];
    FILE *fp = fopen("RestrictedCountries.txt", "r");

    while (fgets(buf, MAXNAMELEN, fp) != NULL) {
            buf[strlen(buf)-1] = '\0';
            restrictedCountries[hash(buf)] = true;
    }
}

// simple hash function
int hash(char *s)
{
    int h;

    for(h = 0; *s != '\0'; s++)
            h = (*s * h + 31) % MAXPAIR;
    return h;
}

void print(struct path *head)
{
    if(head != NULL){
        printPath(head->pathHead);
        printf("length: %d\n----------------------------------------------------------------------\n", head->length);
        print(head->next);
    }
}

void printPath(struct pathItem *head)
{
    if(head != NULL){
        printf("%s ", head->item->name);
        //printf("[%d, %d, %d]\t%s %s %s\n", head->item->isVisited, head->item->isRestrictedCountry, head->item->isRestrictedCity, head->item->name, head->item->country, head->item->city);
        printPath(head->next);
    }
}

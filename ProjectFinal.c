#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

typedef struct CropMinerals
{
    char crop_name[20];
    float salinity_min, salinity_max;
    float nitrogen_min, nitrogen_max;
    float potassium_min, potassium_max;
    float calcium_min, calcium_max;
    float magnesium_min, magnesium_max;
    float water_pH_min, water_pH_max;
    float water_table_min, water_table_max;
} CropData;

CropData crops[] =
    {
        {"Wheat", 0, 4, 1.5, 2.5, 0.5, 1.0, 1.0, 1.5, 0.4, 0.8, 6.5, 7.5, 3.0, 10.0},
        {"Cotton", 0, 5, 1.2, 2.0, 0.4, 0.8, 1.5, 2.0, 0.3, 0.7, 7.0, 8.0, 3.0, 5.0},
        {"Rice", 0, 5, 1.0, 1.8, 0.4, 0.9, 0.8, 1.2, 0.3, 0.6, 6.0, 7.0, 1.5, 5.0},
        {"Sugarcane", 0, 4, 1.2, 2.5, 0.6, 1.0, 1.2, 2.0, 0.3, 0.8, 7.0, 8.0, 3.0, 6.0}};

typedef struct Regions
{
    char crop_name[20];
    char regions[3][20];
} RegionValidity;

RegionValidity Regions[] =
    {
        {"Wheat", {"Punjab", "Sindh", "KPK"}},
        {"Cotton", {"Sindh", "Punjab", "KPK"}},
        {"Rice", {"Punjab", "Sindh", "KPK"}},
        {"Sugarcane", {"Punjab", "Sindh", "KPK"}}};

typedef struct FertilizerPerAcre
{
    float gypsum_per_acre;
    float urea_per_acre;
} Fertilizer;

typedef struct
{
    int day, month, year;
} Date;

int ValidateRegion(char *crop_name, char *region, RegionValidity Regions[], int crop_count);
int ValidateCrop(char *crop_name, CropData crops[], int crop_count);
void AnalysisofSoilWater(CropData crop, float *values, char *region, char *season, Fertilizer fertilizerConfig, FILE *file, float landarea);
void EstimateMinerals(int land_area, float salinity, float nitrogen, FILE *file);
void savereportfile(char *region, char *season, CropData crop, float *values, Date date, FILE *file, int recovery_months, int issues);

int main()
{
    char region[20], season[20], cropname[20];
    int regionchoice, seasonchoice;
    float landarea;

    int cropcount = sizeof(crops) / sizeof(crops[0]);
    int regionscount = sizeof(Regions) / sizeof(Regions[0]);

    Date date;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    date.day = tm.tm_mday;
    date.month = tm.tm_mon + 1;
    date.year = tm.tm_year + 1900;

    printf("Select the region:\n1. Sindh\n2. Punjab\n3. Balochistan\n4. KPK\n");
    printf("Enter: ");
    scanf("%d", &regionchoice);
    getchar();

    switch (regionchoice)
    {
    case 1:
        strcpy(region, "Sindh");
        break;
    case 2:
        strcpy(region, "Punjab");
        break;
    case 3:
        strcpy(region, "Balochistan");
        break;
    case 4:
        strcpy(region, "KPK");
        break;
    default:
        printf("Invalid region choice. Exiting program.\n");
        return 1;
    }

    printf("Select the season:\n1. Winter (December to February)\n2. Spring (March to May)\n3. Summer (June to August)\n4. Autumn (September to November)\n");
    printf("Enter: ");
    scanf("%d", &seasonchoice);
    getchar();

    switch (seasonchoice)
    {
    case 1:
        strcpy(season, "Winter");
        break;
    case 2:
        strcpy(season, "Spring");
        break;
    case 3:
        strcpy(season, "Summer");
        break;
    case 4:
        strcpy(season, "Autumn");
        break;
    default:
        printf("Invalid season choice. Exiting program.\n");
        return 1;
    }

    printf("Enter the area of land being used for farming (in acres): ");
    scanf("%f", &landarea);
    getchar();

    printf("Enter the crop (Wheat, Cotton, Rice, Sugarcane): ");
    fgets(cropname, sizeof(cropname), stdin);
    cropname[strcspn(cropname, "\n")] = '\0';

    CropData selectedcrop;
    if (!ValidateCrop(cropname, crops, cropcount))
    {
        printf("Error: The crop '%s' is invalid.\n", cropname);
        return 1;
    }

    for (int i = 0; i < cropcount; i++)
    {
        if (strcmp(cropname, crops[i].crop_name) == 0)
        {
            selectedcrop = crops[i];
            break;
        }
    }

    if (ValidateRegion(cropname, region, Regions, regionscount))
    {
        printf("The crop '%s' can grow in region '%s'.\n", cropname, region);
    }
    else
    {
        printf("Error: The crop '%s' does not grow in region '%s'.\n", cropname, region);
        return 1;
    }

    float values[7];
    printf("Enter Soil Salinity (dS/m): ");
    scanf("%f", &values[0]);
    printf("Enter Nitrogen (%%): ");
    scanf("%f", &values[1]);
    printf("Enter Potassium (%%): ");
    scanf("%f", &values[2]);
    printf("Enter Calcium (%%): ");
    scanf("%f", &values[3]);
    printf("Enter Magnesium (%%): ");
    scanf("%f", &values[4]);
    printf("Enter Water pH: ");
    scanf("%f", &values[5]);
    printf("Enter Water Table Depth (feet): ");
    scanf("%f", &values[6]);

    int recoverymonths = 0;
    int issues = 0;

    Fertilizer fertilizerConfig = {500.0, 50.0};

    FILE *file = fopen("report.txt", "w");
    if (file == NULL)
    {
        printf("Unable to open the file for writing.\n");
        return 1;
    }

    AnalysisofSoilWater(selectedcrop, values, region, season, fertilizerConfig, file, landarea);
    // Estimate Minerals will be passed throught Analyze Functions
    savereportfile(region, season, selectedcrop, values, date, file, recoverymonths, issues);
    fclose(file);

    return 0;
}
int ValidateCrop(char *crop_name, CropData crops[], int crop_count)
{
    for (int i = 0; i < crop_count; i++)
    {
        if (strcmp(crop_name, crops[i].crop_name) == 0)
        {
            return 1;
        }
    }
    return 0;
}
int ValidateRegion(char *crop_name, char *region, RegionValidity Regions[], int region_count)
{
    for (int i = 0; i < region_count; i++)
    {
        if (strcmp(crop_name, Regions[i].crop_name) == 0)
        {
            for (int j = 0; j < 3; j++)
            {
                if (strcmp(region, Regions[i].regions[j]) == 0)
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}
void AnalysisofSoilWater(CropData crop, float *values, char *region, char *season, Fertilizer fertilizerConfig, FILE *file, float landarea)
{
    int severity = 0;

    // Compare each parameter with thresholds
    if (values[0] > crop.salinity_max)
    {
        fprintf(file, "High salinity detected. Apply %.2f kg of gypsum per acre.\n", fertilizerConfig.gypsum_per_acre);
        severity++;
    }
    if (values[1] < crop.nitrogen_min)
    {
        fprintf(file, "Low nitrogen detected. Add %.2f kg of urea fertilizer per acre.\n", fertilizerConfig.urea_per_acre);
        severity++;
    }
    // Repeat for other parameters (Potassium, Calcium, etc.)

    // Severity grading
    if (severity == 0)
    {
        fprintf(file, "Soil condition: Fertile\n");
    }
    else if (severity <= 3)
    {
        fprintf(file, "Soil condition: Sub-Fertile\n");
    }
    else
    {
        fprintf(file, "Soil condition: Unfertile\n");
    }
}
void EstimateMinerals(int land_area, float salinity, float nitrogen, FILE *file)
{
    float potassium = (land_area * 0.498) - (salinity * 0.12) + (nitrogen * 0.21);
    float phosphorus = (land_area * 0.31) - (salinity * 0.04) + (nitrogen * 0.17);
    float calcium = (land_area * 0.6) - (salinity * 0.4) + (nitrogen * 0.089);
    float magnesium = (land_area * 0.22) - (salinity * 0.14) + (nitrogen * 0.052);
    if (file != NULL)
    {
        fprintf(file, "Mineral Estimates:\n");
        fprintf(file, "Potassium: %.2f\n", potassium);
        fprintf(file, "Phosphorus: %.2f\n", phosphorus);
        fprintf(file, "Calcium: %.2f\n", calcium);
    }
    else
    {
        printf("No file.\n");
    }
}
void savereportfile(char *region, char *season, CropData crop, float *values, Date date, FILE *file, int recovery_months, int issues)
{
    if (file != NULL)
    {
        fprintf(file, "Report of Region: %s\n", region);
        fprintf(file, "Season: %s\n", season);
        fprintf(file, "Crop_Name: %s\n", crop);
        fprintf(file, "The Values are: \n");

        for (int i = 0; i < 5; i++)
        {
            fprintf(file, "Value %d: %.2f\n", i + 1, values[i]);
        }
        fprintf(file, "Date: %02d/%02d/%04d\n", date.day, date.month, date.year);
        fprintf(file, "Recovery_Months: %d\n", recovery_months);
        fprintf(file, "Issues are: %d\n", issues);
    }
    else
    {
        printf("No file.\n");
    }
}

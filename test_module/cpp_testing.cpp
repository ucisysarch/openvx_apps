#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>

void checkminmax(FILE* f, int **currentarr, int **beforearr, int **nextarr, int arraysize);
bool is_extremum(FILE* f, int **currentarr, int **beforearr, int **nextarr, int xpos, int ypos);

int main()
{
	std::fstream test1_1;
	std::fstream test1_2;
	std::fstream test1_3;
	
	std::fstream test2_1;
	std::fstream test2_2;
	std::fstream test2_3;

	std::fstream test3_1;
	std::fstream test3_2;
	std::fstream test3_3;

	std::fstream test4_1;
	std::fstream test4_2;
	std::fstream test4_3;

	std::fstream test5_1;
	std::fstream test5_2;
	std::fstream test5_3;
	
	test1_1.open("test1_1.txt");
	test1_2.open("test1_2.txt");
	test1_3.open("test1_3.txt");

	test2_1.open("test2_1.txt");
	test2_2.open("test2_2.txt");
	test2_3.open("test2_3.txt");

	test3_1.open("test3_1.txt");
	test3_2.open("test3_2.txt");
	test3_3.open("test3_3.txt");

	test4_1.open("test4_1.txt");
	test4_2.open("test4_2.txt");
	test4_3.open("test4_3.txt");

	test5_1.open("test5_1.txt");
	test5_2.open("test5_2.txt");
	test5_3.open("test5_3.txt");

	int test1_size;
	test1_1 >> test1_size;
	test1_1 >> test1_size;
	test1_2 >> test1_size;
	test1_2 >> test1_size;
	test1_3 >> test1_size;
	test1_3 >> test1_size;

	int test2_size;
	test2_1 >> test2_size;
	test2_1 >> test2_size;
	test2_2 >> test2_size;
	test2_2 >> test2_size;
	test2_3 >> test2_size;
	test2_3 >> test2_size;
	
	int test3_size;
	test3_1 >> test3_size;
	test3_1 >> test3_size;
	test3_2 >> test3_size;
	test3_2 >> test3_size;
	test3_3 >> test3_size;
	test3_3 >> test3_size;

	int test4_size;
	test4_1 >> test4_size;
	test4_1 >> test4_size;
	test4_2 >> test4_size;
	test4_2 >> test4_size;
	test4_3 >> test4_size;
	test4_3 >> test4_size;
	
	int test5_size;
	test5_1 >> test5_size;
	test5_1 >> test5_size;
	test5_2 >> test5_size;
	test5_2 >> test5_size;
	test5_3 >> test5_size;
	test5_3 >> test5_size;

	int **i_test1_1 = new int*[test1_size];
	int **i_test1_2 = new int*[test1_size];
	int **i_test1_3 = new int*[test1_size];

	for (int i = 0; i < test1_size; i++)
	{
		i_test1_1[i] = new int[test1_size];
		i_test1_2[i] = new int[test1_size];
		i_test1_3[i] = new int[test1_size];
		for (int j = 0; j < test1_size; j++)
		{
			test1_1 >> i_test1_1[i][j];
			test1_2 >> i_test1_2[i][j];
			test1_3 >> i_test1_3[i][j];
		}
	}

	int **i_test2_1 = new int*[test2_size];
	int **i_test2_2 = new int*[test2_size];
	int **i_test2_3 = new int*[test2_size];

	for (int i = 0; i < test2_size; i++)
	{
		i_test2_1[i] = new int[test2_size];
		i_test2_2[i] = new int[test2_size];
		i_test2_3[i] = new int[test2_size];
		for (int j = 0; j < test2_size; j++)
		{
			test2_1 >> i_test2_1[i][j];
			test2_2 >> i_test2_2[i][j];
			test2_3 >> i_test2_3[i][j];
		}
	}

	int **i_test3_1 = new int*[test3_size];
	int **i_test3_2 = new int*[test3_size];
	int **i_test3_3 = new int*[test3_size];

	for (int i = 0; i < test3_size; i++)
	{
		i_test3_1[i] = new int[test3_size];
		i_test3_2[i] = new int[test3_size];
		i_test3_3[i] = new int[test3_size];
		for (int j = 0; j < test3_size; j++)
		{
			test3_1 >> i_test3_1[i][j];
			test3_2 >> i_test3_2[i][j];
			test3_3 >> i_test3_3[i][j];
		}
	}

	int **i_test4_1 = new int*[test4_size];
	int **i_test4_2 = new int*[test4_size];
	int **i_test4_3 = new int*[test4_size];

	for (int i = 0; i < test4_size; i++)
	{
		i_test4_1[i] = new int[test4_size];
		i_test4_2[i] = new int[test4_size];
		i_test4_3[i] = new int[test4_size];
		for (int j = 0; j < test4_size; j++)
		{
			test4_1 >> i_test4_1[i][j];
			test4_2 >> i_test4_2[i][j];
			test4_3 >> i_test4_3[i][j];
		}
	}

	int **i_test5_1 = new int*[test5_size];
	int **i_test5_2 = new int*[test5_size];
	int **i_test5_3 = new int*[test5_size];

	for (int i = 0; i < test5_size; i++)
	{
		i_test5_1[i] = new int[test5_size];
		i_test5_2[i] = new int[test5_size];
		i_test5_3[i] = new int[test5_size];
		for (int j = 0; j < test5_size; j++)
		{
			test5_1 >> i_test5_1[i][j];
			test5_2 >> i_test5_2[i][j];
			test5_3 >> i_test5_3[i][j];
		}
	}

	/*
	printf("\n");
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			printf("%d ", i_test1_1[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			printf("%d ", i_test1_2[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			printf("%d ", i_test1_3[i][j]);
		}
		printf("\n");
	}
	*/


	FILE* f;
	f = fopen("output1.txt", "w");
	std::cout << "Case 1" << std::endl;
	checkminmax(f, i_test1_2, i_test1_1, i_test1_3, test1_size);
	fclose(f);

	f = fopen("output2.txt", "w");
	std::cout << "Case 2" << std::endl;
	checkminmax(f, i_test2_2, i_test2_1, i_test2_3, test2_size);
	fclose(f);

	f = fopen("output3.txt", "w");
	std::cout << "Case 3" << std::endl;
	checkminmax(f, i_test3_2, i_test3_1, i_test3_3, test3_size);
	fclose(f);

	f = fopen("output4.txt", "w");
	std::cout << "Case 4" << std::endl;
	checkminmax(f, i_test4_2, i_test4_1, i_test4_3, test4_size);
	fclose(f);

	f = fopen("output5.txt", "w");
	std::cout << "Case 5" << std::endl;
	checkminmax(f, i_test5_2, i_test5_1, i_test5_3, test5_size);
	fclose(f);
}

void checkminmax(FILE* f, int **currentarr, int **beforearr, int **nextarr, int arraysize)
{
	int threshold = 1;

	for (int i = threshold; i<arraysize - threshold; i++)
	{
		for (int j = threshold; j < arraysize - threshold; j++)
		{
			if (is_extremum(f, currentarr, beforearr, nextarr, i, j))
			{
				fprintf(f, "%d %d %d\n", currentarr[i][j], j, i);
			}
		}
	}
}

bool is_extremum(FILE* f, int **currentarr, int **beforearr, int **nextarr, int xpos, int ypos)
{
	int val = currentarr[xpos][ypos];


	int curneb[8] =
	{
		currentarr[xpos - 1][ypos - 1],
		currentarr[xpos][ypos - 1],
		currentarr[xpos + 1][ypos - 1],
		currentarr[xpos - 1][ypos],
		currentarr[xpos + 1][ypos],
		currentarr[xpos - 1][ypos + 1],
		currentarr[xpos][ypos + 1],
		currentarr[xpos + 1][ypos + 1]
	};

	int beforeneb[9] =
	{
		beforearr[xpos - 1][ypos - 1],
		beforearr[xpos][ypos - 1],
		beforearr[xpos + 1][ypos - 1],
		beforearr[xpos - 1][ypos],
		beforearr[xpos][ypos],
		beforearr[xpos + 1][ypos],
		beforearr[xpos - 1][ypos + 1],
		beforearr[xpos][ypos + 1],
		beforearr[xpos + 1][ypos + 1]
	};
	int nextneb[9] =
	{
		nextarr[xpos - 1][ypos - 1],
		nextarr[xpos][ypos - 1],
		nextarr[xpos + 1][ypos - 1],
		nextarr[xpos - 1][ypos],
		nextarr[xpos][ypos],
		nextarr[xpos + 1][ypos],
		nextarr[xpos - 1][ypos + 1],
		nextarr[xpos][ypos + 1],
		nextarr[xpos + 1][ypos + 1]
	};

	if ((val >= curneb[0] && val >= curneb[1] && val >= curneb[2] && val >= curneb[3] && val >= curneb[4] && val >= curneb[5] && val >= curneb[6]
		&& val >= curneb[7] && val >= beforeneb[0] && val >= beforeneb[1] && val >= beforeneb[2] && val >= beforeneb[3]
		&& val >= beforeneb[4] && val >= beforeneb[5] && val >= beforeneb[6] && val >= beforeneb[7] && val >= beforeneb[8]
		&& val >= nextneb[0] && val >= nextneb[1] && val >= nextneb[2] && val >= nextneb[3] && val >= nextneb[4] && val >= nextneb[5]
		&& val >= nextneb[6] && val >= nextneb[7] && val >= nextneb[8]) || (val <= curneb[0] && val <= curneb[1] && val <= curneb[2] && val <= curneb[3] && val <= curneb[4] && val <= curneb[5] && val <= curneb[6]
		&& val <= curneb[7] && val <= beforeneb[0] && val <= beforeneb[1] && val <= beforeneb[2] && val <= beforeneb[3]
		&& val <= beforeneb[4] && val <= beforeneb[5] && val <= beforeneb[6] && val <= beforeneb[7] && val <= beforeneb[8]
		&& val <= nextneb[0] && val <= nextneb[1] && val <= nextneb[2] && val <= nextneb[3] && val <= nextneb[4] && val <= nextneb[5]
		&& val <= nextneb[6] && val <= nextneb[7] && val <= nextneb[8]))
	{
		return true;
		//fprintf(f, "%d %d %d\n", val, xpos, ypos);
	}

	/* check for maximum 
	if (val > 0)
	{
		for (int i = 0; i < 8; i++)
		{
			if (val < curneb[i])
				return false;
		}
		for (int i = 0; i < 9; i++)
		{
			if (val < beforeneb[i])
				return false;
		}
		for (int i = 0; i < 9; i++)
		{
			if (val < nextneb[i])
				return false;
		}
		//std::cout << "Maximum :";
		return true;
	}
	
	else
	{
		for (int i = 0; i < 8; i++)
		{
			if (val > curneb[i])
				return false;
		}
		for (int i = 0; i < 9; i++)
		{
			if (val > beforeneb[i])
				return false;
		}
		for (int i = 0; i < 9; i++)
		{
			if (val > nextneb[i])
				return false;
		}
		//std::cout << "Mimimum :";
		return true;
	}
	*/
	return false;

	
}
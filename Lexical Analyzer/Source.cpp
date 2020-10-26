# include <iostream>
# include <fstream>
# include <string>

using namespace std;

int table[35][21];
int accept[35];
int ascii[128];

void load_arr()
{
	int x, y = 0;

	for (int i = 0; i < 128; i++)
	{
		ascii[i] = -1;
		if (i < 35)
			accept[i] = -1;
	}

	ifstream read;
	read.open("ascii.txt", ios::in);

	for (int i = 0; i < 128; i++)
	{
		read >> x >> y;
		if (x == i)
			ascii[i] = y;
	}

	read.close();

	read.open("accept.txt", ios::in);

	for (int i = 0; i < 35; i++)
	{
		read >> x;
		accept[i] = x;
	}
	read.close();

	read.open("table.txt", ios::in);

	for (int i = 0; i < 35; i++)
	{
		for (int j = 0; j < 21; j++)
		{
			read >> x;
			table[i][j] = x;
		}
	}
	read.close();
}

bool is_keyword(string arr1)
{
	string arr[10] = { "int", "float", "#include", "#define", "cin>>", "if", "while", "for", "do", "cout<<" };

	bool flag = false;
	for (int i = 0; i < 10; i++)
	{
		if (arr[i] == arr1)
		{
			flag = true;
			break;
		}
	}

	return flag;
}

void create_token(string word, string type)
{
	ofstream write;
	write.open("token.txt", ios::app);

	write << word << " " << type << endl;
	write.close();
}

bool check_error(int & state, char c, int & index, int & pindex, string line, int & count, int &  eno)
{
	char var;

	if (line[index] != '\0' && line[index + 1] != '\0')
		var = line[index + 1];
	else
		var = c;

	if (table[state - 1][ascii[c]] == -1 && table[state - 1][ascii[var]] == -1)
	{
		ofstream write;
		string word;
		eno++;


		for (int i = 0, j = pindex; j <= index; j++, i++)
			word = word + line[j];

		write.open("error.txt", ios::app);
		write << "error no:" << eno << " on line number: " << count << " " << word << endl;
		write.close();

		pindex = index;
		state = 1;
		return true;
	}
	else
		return false;
}

bool is_accept(int & state, char c, int & index, int & pindex, string line, int & lcount, int& ecount)
{
	if (accept[state - 1] != 0)
	{
		string word;
		word[0] = '\0';
		if (accept[state - 1] > 0)
		{
			for (int i = 0, j = pindex; line[j] != '\0' && line[j] != ' ' && j <= index; j++, i++)
				word = word + line[j];
			index++;
		}
		else
		{
			// index--; 
			for (int i = 0, j = pindex; line[j] != '\0' && line[j] != ' ' && j <= index; j++, i++)
				word = word + line[j];

		}
		if (accept[state - 1] == 1 || accept[state - 1] == -1)
			create_token(word, "identifier");
		else if (accept[state - 1] == 2 || accept[state - 1] == -2)
			create_token(word, "number");
		else if (accept[state - 1] == 3 || accept[state - 1] == -3)
			create_token(word, "operator");
		else if (accept[state - 1] == 4 || accept[state - 1] == -4)
			create_token(word, "punctuation");
		else if (accept[state - 1] == 5 || accept[state - 1] == -5)
		{
			if (is_keyword(word))
			{
				create_token(word, "keyword");
			}
			else
			{
				ofstream write;
				ecount++;
				write.open("error.txt", ios::app);
				write << "Error no:" << ecount << " on line number: " << lcount << " " << word << endl;
				write.close();
				state = 1;
				pindex = ++index;
				return true;
			}

		}
		state = 1;
		pindex = index;
		return true;
	}
	else
		return false;
}

int main()
{
	load_arr();

	string line;
	int lcount = 0;
	int ecount = 0;
	int index = 0;
	int pindex = 0;
	int state = 0;


	bool flag = false;
	bool check = false;
	bool accepted = false;

	char c = '\0';

	ifstream read;
	ofstream write;

	write.open("token.txt", ios::out);
	write.close();

	write.open("error.txt", ios::out);
	write.close();


	read.open("read.txt", ios::in);

	getline(read, line);
	lcount++;

	while (!read.eof())
	{
		state = 1;
		while (1)
		{
			while (1)
			{
				c = line[index];
				if (c != ' ' && c != '\0')
				{
					flag = true;
					break;
				}
				else if (c == ' ')
				{
					int nstate = table[state - 1][ascii[c]];
					if (accept[nstate - 1] != 0)
					{
						is_accept(nstate, c, index, pindex, line, lcount, ecount);
						state = nstate;
					}

					index++;
					pindex = index;
				}
				else if (c == '\0')
				{
					if (accepted == false)
					{

						if (table[state - 1][ascii[c]] != -1)
						{
							state = table[state - 1][ascii[c]];
							if (!is_accept(state, c, index, pindex, line, lcount, ecount))
							{
								if (check_error(state, c, index, pindex, line, lcount, ecount))
									check = true;
							}
							else
								check = true;
						}
						else
						{
							if (check_error(state, c, index, pindex, line, lcount, ecount))
								check = true;
						}
					}
					else
						check = true;

				}
				if (check == true)
					break;

			}

			if (check == true)
				break;

			accepted = false;

			state = table[state - 1][ascii[c]];

			if (!is_accept(state, c, index, pindex, line, lcount, ecount))
			{
				//if (!check_error(state, c, index, pindex, line, lcount, ecount))
				index++;
				//else
				//accepted = true; 
			}
			else
				accepted = true;

			flag = false;


		}
		getline(read, line);
		check = false;
		lcount++;
		pindex = 0;
		index = 0;
	}
	read.close();
	return 0;
}
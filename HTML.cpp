#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
using std::cout;
using std::cin;
using std::endl;

#pragma warning(disable : 4996)

namespace GlobalConsts
{
	const unsigned short MAX_CELL_LEN = 50;
	const unsigned short MAX_ROWS_IN_TABLE = 100;
	const unsigned short MAX_CELLS_PER_ROW = 15;
	const unsigned short MAX_FILENAME_LEN = 50;
	const unsigned short MAX_CELL_LEN_CER = 5 * MAX_CELL_LEN; // *5 if all symbols written in Character Entity Reference (CER) and between 100-127
	const char ROW_TAG[] = "tr";
	const char ROW_CLOSE_TAG[] = "/tr";

	const char HEADER_TAG[] = "th";
	const char HEADER_CLOSE_TAG[] = "/th";
	const char CELL_TAG[] = "td";
	const char CELL_CLOSE_TAG[] = "/td";
	const char ERROR_TAG[] = "error";
	const unsigned short MAX_TAG_LEN = 10;
	const char HALF_TAB[] = "  ";
	const char TAB[] = "    ";
}
using namespace GlobalConsts;

namespace Functions
{
	bool isBetween(int num, int lowerBorder, int upperBorder)
	{
		return lowerBorder <= num && num <= upperBorder;
	}
	bool isDigit(char symbol)
	{
		return isBetween(symbol, '0', '9');
	}
	char convertDigitsToSymbolFromFile(std::stringstream& ifs)
	{
		if (isDigit(ifs.peek()))
		{
			short number = ifs.get() - '0';
			if (isDigit(ifs.peek()))
			{
				number = number * 10 + ifs.get() - '0';
				if (isBetween(number, 32, 99)) //printable ASCII codes 2 digits
				{
					return number;
				}
				if (isDigit(ifs.peek()))
				{
					number = number * 10 + ifs.get() - '0';
					if (isBetween(number, 32, 127)) //printable ASCII codes 3 digits
					{
						return number;
					}
				}
			}
		}
		return ' ';
	}
	void setWfromLeftToRight(const char* text, unsigned short setWInterval, char filler)
	{
		cout << text;
		unsigned short fillingIndex = setWInterval - strlen(text);
		for (size_t i = 0; i < fillingIndex; i++)
		{
			cout << filler;
		}
	}
}
using namespace Functions;
class Cell
{
	char cellText[MAX_CELL_LEN + 1] = {}; //* at start and * and the end if header and '\0'
	bool isHeader = false;

public:
	void clear()
	{
		this->cellText[0] = '\0';
		isHeader = false;
	}
	void changeCell(const char* cellText,bool isHeader)
	{
		strcpy(this->cellText, cellText);
		this->isHeader = isHeader;
	}
	const char* getText() const
	{
		return this->cellText;
	}
	bool IsHeader() const
	{
		return this->isHeader;
	}
};

class Row
{
	Cell Columns[MAX_CELLS_PER_ROW];
	unsigned short columnsCount = 0;

	void removeCharacterEntityReference(char* str) const
	{
		if (!str)
		{
			return;
		}
		unsigned short sizeStr = strlen(str);
		char* resultStr = new char[sizeStr + 1]; //for '\0'
		unsigned short index = 0;
		std::stringstream ss(str);
		while (true)
		{
			char symbol = ss.get();
			if (!ss.good())
			{
				break;
			}
			if (symbol == '&' && ss.peek() == '#')
			{
				ss.get();//remove #
				symbol = convertDigitsToSymbolFromFile(ss);
			}
			resultStr[index++] = symbol;
		}
		resultStr[index] = '\0';
		strcpy(str, resultStr);
		delete[] resultStr;
		
	}
	
public:
	void setColumnCount(int colCount)
	{
		if (!isBetween(colCount, 0, MAX_CELLS_PER_ROW))
		{
			return;
		}
		columnsCount = colCount;
	}
	void columnsCountPlus1()
	{
		++columnsCount;
	}

	void readTextAndSetInCell(std::ifstream& file, bool isHeader)
	{
		char buff[MAX_CELL_LEN_CER + 1];//'\0'
		file.get(buff, MAX_CELL_LEN_CER + 1, '<');
		removeCharacterEntityReference(buff);
		setCell(columnsCount, buff, isHeader);

	}
	void setCell(short position, char* text, bool isHeader)
	{
		if (!isBetween(position, 0, MAX_CELLS_PER_ROW - 1) || !text)
		{
			return;
		}
		if (strlen(text) > MAX_CELL_LEN)
		{
			text[MAX_CELL_LEN] = '\0';
		}
		Columns[position].changeCell(text, isHeader);
	}
	const Cell& getCell(short position) const
	{
		return Columns[position];
	}
	unsigned short getColumnsCount() const
	{
		return this->columnsCount;
	}
	void clear()
	{
		for (size_t i = 0; i < columnsCount; i++)
		{
			this->Columns[i].clear();
		}
		columnsCount = 0;
	}
};

class Table
{
	Row rowsTable[MAX_ROWS_IN_TABLE];
	unsigned short rowsCount = 0;
	bool isValidTable = false;
	
	const char* readNextTag(std::ifstream& file) const
	{
		while (true)
		{
			char symbol = 0;
			symbol = file.get();
			if (file.fail() && !file.eof())
			{
				file.clear();
			}
			if (!file.good())
			{
				return ERROR_TAG;
			}
			if (symbol != '<')
			{
				continue;
			}
			char buff[MAX_TAG_LEN];
			file.getline(buff, MAX_TAG_LEN, '>');
			return buff;
		}
		
	}
	void loadTable(std::ifstream& file)
	{
		while (true)
		{
			char tag[MAX_TAG_LEN];
			strcpy(tag, readNextTag(file));
			if (!strcmp(tag, ROW_TAG))
			{
				rowsCount++;
			}
			else if (!strcmp(tag, ERROR_TAG))
			{
				return;
			}
			else if (!strcmp(tag, CELL_TAG))
			{
				rowsTable[rowsCount - 1].readTextAndSetInCell(file, false);
			}
			else if (!strcmp(tag, HEADER_TAG))
			{
				rowsTable[rowsCount - 1].readTextAndSetInCell(file, true);
			}
			else if (!strcmp(tag, HEADER_CLOSE_TAG) || !strcmp(tag, CELL_CLOSE_TAG))
			{
				rowsTable[rowsCount - 1].columnsCountPlus1();
			}
			
		}
	}

	unsigned short findMaxColsInTable() const
	{
		unsigned short maxColsCount = 0;
		for (size_t i = 0; i < rowsCount; i++)
		{
			if (rowsTable[i].getColumnsCount() > maxColsCount)
			{
				maxColsCount = rowsTable[i].getColumnsCount();
			}
		}
		return maxColsCount;
	}
	void fillCellLenArr( unsigned short* cellLenArr, unsigned short size) const
	{
		for (size_t i = 0; i < size; i++)
		{
			unsigned short maxColLen = 0;
			for (size_t j = 0; j < rowsCount; j++)
			{
				if (strlen(rowsTable[j].getCell(i).getText()) > maxColLen)
				{
					maxColLen = strlen(rowsTable[j].getCell(i).getText());
				}
			}
			cellLenArr[i] = maxColLen;
		}
	}
	void printSymbolHeader(bool header) const
	{
		if (header)
		{
			cout << '*';
		}
		else
		{
			cout << ' ';
		}
	}
	void clear()
	{
		for (size_t i = 0; i < rowsCount; i++)
		{
			rowsTable[i].clear();
		}
		rowsCount = 0;
	}
public:
	Table() = default;
	Table(const char* fileName)
	{
		load(fileName);
	}
	void load(const char* fileName)
	{
		clear();
		if (!fileName)
		{
			isValidTable = false;
			return;
		}
		std::ifstream ifs(fileName);
		if (!ifs.is_open())
		{
			isValidTable = false;
			return;
		}
		loadTable(ifs);
		isValidTable = true;
		ifs.close();
	}
	void save(const char* saveInFileName) const
	{
		if (!saveInFileName || !isValidTable)
		{
			return;
		}
		std::ofstream ofs(saveInFileName);
		if (!ofs.is_open())
		{
			cout << "File not saved.";
			return;
		}
		ofs << "<table>" << endl;
		for (size_t i = 0; i < rowsCount; i++)
		{
			ofs << HALF_TAB << '<' << ROW_TAG << '>' << endl;
			for (size_t j = 0; j < rowsTable[i].getColumnsCount(); j++)
			{
				if (rowsTable[i].getCell(j).IsHeader())
				{
					ofs << TAB << '<' << HEADER_TAG << '>' << rowsTable[i].getCell(j).getText() << '<' << HEADER_CLOSE_TAG << '>' << endl;
				}
				else
				{
					ofs << TAB << '<' << CELL_TAG << '>' << rowsTable[i].getCell(j).getText() << '<' << CELL_CLOSE_TAG << '>' << endl;
				}
				
			}
			ofs << HALF_TAB << '<' << ROW_CLOSE_TAG << '>' << endl;
		}
		ofs << "</table>";
		ofs.close();
	}
	void edit(int rowNumber, int colNumber, char* newText)
	{
		if (!newText)
		{
			return;
		}
		if (!isBetween(rowNumber, 1, MAX_ROWS_IN_TABLE) || !isBetween(colNumber, 1, MAX_CELLS_PER_ROW))
		{
			return;
		}
		bool notChangingHeader = rowsTable[rowNumber - 1].getCell(colNumber - 1).IsHeader();
		rowsTable[rowNumber - 1].setCell(colNumber - 1, newText, notChangingHeader);

		if (rowNumber > rowsCount)
		{
			rowsCount = rowNumber;
		}
		if (colNumber > rowsTable[rowNumber - 1].getColumnsCount())
		{
			rowsTable[rowNumber - 1].setColumnCount(colNumber);
		}
	}

	void print() const
	{
		unsigned short columnsTable = findMaxColsInTable();
		unsigned short* maxCellLenInEveryColumn = new unsigned short[columnsTable];
		fillCellLenArr(maxCellLenInEveryColumn, columnsTable);
		cout << endl;
		for (size_t i = 0; i < rowsCount; i++)
		{
			cout << '|';
			for (size_t j = 0; j < columnsTable; j++)
			{
				printSymbolHeader(rowsTable[i].getCell(j).IsHeader());
				
				setWfromLeftToRight(rowsTable[i].getCell(j).getText(), maxCellLenInEveryColumn[j], ' ');

				printSymbolHeader(rowsTable[i].getCell(j).IsHeader());
				
				cout << '|';
			}
			cout << endl;
		}
		delete[] maxCellLenInEveryColumn;
	}

	void addRow(int rowIndex, const char* rowFill) // Every White space separates different CellFillers
	{
		if (!rowFill || !isBetween(rowIndex, 1, MAX_ROWS_IN_TABLE - 1))
		{
			return;
		}
		for (size_t i = MAX_ROWS_IN_TABLE - 1; i >= rowIndex; i--)
		{
			std::swap(rowsTable[i], rowsTable[i - 1]);
		}
		rowsCount++;
		rowsTable[rowIndex - 1].clear();
		std::stringstream ss(rowFill);
		for (size_t i = 0; i < MAX_CELLS_PER_ROW; i++)
		{
			if (ss.eof())
			{
				break;
			}
			char buff[MAX_CELL_LEN];
			ss.getline(buff, MAX_CELL_LEN, ' ');
			rowsTable[rowIndex - 1].setCell(i, buff, false);
			rowsTable[rowIndex - 1].columnsCountPlus1();
		}
	}

	void removeRow(int rowIndex)
	{
		if (!isBetween(rowIndex, 1, MAX_ROWS_IN_TABLE))
		{
			return;
		}
		for (size_t i = rowIndex - 1; i < MAX_ROWS_IN_TABLE - 1; i++)
		{
			std::swap(rowsTable[i], rowsTable[i + 1]);
		}
		rowsTable[MAX_ROWS_IN_TABLE - 1].clear();
		rowsCount--;
	}
};
int main()
{
	cout << "Load valid table" << endl << '>';
	char HTMLtableName[GlobalConsts::MAX_FILENAME_LEN];
	cin >> HTMLtableName;
	Table t(HTMLtableName); 
	t.print();
	t.save("newFileTest.txt");
}
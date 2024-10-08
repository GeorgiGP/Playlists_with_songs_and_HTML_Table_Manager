#include <iostream>
#include <fstream>
#include <iomanip>

#pragma warning(disable : 4996)
typedef char Byte;
namespace GlobalConsts
{
	const uint16_t MAX_PLAYLIST_SONGS = 30;
	const uint16_t MAX_SONGNAME_LEN = 64 + 1; //terminating 0
	const uint16_t MAX_SOUNDS_CONTENT = 256;
	const uint16_t MINS_IN_HOUR = 60, SECONDS_IN_MIN = 60, SECONDS_IN_HOUR = 60 * 60;

	const uint16_t ALL_KNOWN_GENRES = 5;
}
enum class Genre : unsigned char
{
	UNKNOWN, //0
	ROCK = 1 << 0,
	POP = 1 << 1,
	HIP_HOP = 1 << 2,
	EDM = 1 << 3,
	JAZZ = 1 << 4,
	OTHER = 1 << 5
};
namespace Functions
{
	Genre symbolToGenre(const char symbol)
	{
		switch (symbol)
		{
		case 'r':
		case 'R':
			return Genre::ROCK;
		case 'p':
		case 'P':
			return Genre::POP;
		case 'h':
		case 'H':
			return Genre::HIP_HOP;
		case 'e':
		case 'E':
			return Genre::EDM;
		case 'j':
		case 'J':
			return Genre::JAZZ;
		default:
			return Genre::OTHER;
		}
	}
	Genre strToGenre(const char* genreStr)
	{
		if (!genreStr)
		{
			return Genre::UNKNOWN;
		}
		Genre convertion = Genre::UNKNOWN;
		while (*genreStr)
		{
			convertion = (Genre)((unsigned char)convertion | (unsigned char)symbolToGenre(*genreStr));
			genreStr++;
		}
		return convertion;
	}
	int32_t min(const int32_t num1, const int32_t num2)
	{
		if (num1 <= num2)
		{
			return num1;
		}
		return num2;
	}
	void printGenre(const Genre genre)
	{
		bool isPrinted = false;
		for (size_t i = 0; i < GlobalConsts::ALL_KNOWN_GENRES; i++)
		{
			if ((unsigned char)genre & (1 << i))
			{
				if (!isPrinted)
				{
					isPrinted = true;
				}
				else
				{
					std::cout << '&';
				}
				switch (i)
				{
				case 0:
					std::cout << "Rock";
					break;
				case 1:
					std::cout << "Pop";
					break;
				case 2:
					std::cout << "Hip-Hop";
					break;
				case 3:
					std::cout << "EDM";
					break;
				case 4:
					std::cout << "JAZZ";
					break;
				}
				
			}
		}
	}
	 
	void charArrCpy(char* const destArr, const char* const sourceArr, const uint32_t sourseArrSize)
	{
		if (!destArr || !sourceArr)
		{
			return;
		}
		for (size_t i = 0; i < sourseArrSize; i++)
		{
			destArr[i] = sourceArr[i];
		}
	}

	uint16_t countDitgits(uint32_t num)
	{
		uint16_t count = 1;
		num /= 10; //if 0 countDigits = 1
		while (num > 0)
		{
			num /= 10;
			count++;
		}
		return count;
	}
}

struct Time
{
private:
	uint32_t seconds = 0;

	bool validateAndSet(int32_t lowerBorder, int32_t upperBorder, uint32_t newValue, uint32_t oldValue, const uint16_t multiplier)
	{
		if (lowerBorder > newValue || newValue > upperBorder)
		{
			return false;
		}
		this->seconds += newValue * multiplier;
		this->seconds -= oldValue * multiplier;
		return true;
	}
public:
	Time()
	{
		Time(0, 0, 0);
	}
	Time(const int32_t hours, const int32_t mins, const int32_t seconds)
	{
		setHours(hours);
		setMins(mins);
		setSeconds(seconds);
	}
	const uint32_t getHours() const
	{
		return this->seconds / GlobalConsts::SECONDS_IN_HOUR;
	}
	const uint32_t getMins() const
	{
		return (this->seconds % GlobalConsts::SECONDS_IN_HOUR) / GlobalConsts::SECONDS_IN_MIN;
	}
	const uint32_t getSeconds() const
	{
		return this->seconds % GlobalConsts::SECONDS_IN_MIN;
	}
	bool setHours(const int32_t hours)
	{
		return validateAndSet(0, 48, hours, getHours(), GlobalConsts::SECONDS_IN_HOUR); //2 days
	}
	bool setMins(const int32_t mins)
	{
		return validateAndSet(0, 60, mins, getMins(), GlobalConsts::SECONDS_IN_MIN);
	}
	bool setSeconds(const int32_t seconds) 
	{
		return validateAndSet(0, 60, seconds, getSeconds(), 1);
	}
	void print() const
	{
		std::cout << std::setw(2) << std::setfill('0') << getHours() << ':' <<
					std::setw(2) << std::setfill('0') << getMins() << ':' <<
					std::setw(2) << std::setfill('0') << getSeconds();
	}
};

struct Content
{
private:
	Byte sounds[GlobalConsts::MAX_SOUNDS_CONTENT] = {};
	uint16_t sizeOfContent = 0;
public:
	Content() = default;
	Content(const Byte* const sounds,const int16_t sizeOfContent) 
	{
		this->setSoundsSize(sounds, sizeOfContent);
	}
	const Byte* getSounds() const
	{
		return this->sounds;
	}
	uint16_t getSizeContent() const
	{
		return this->sizeOfContent;
	}
	void setSoundsSize(const Byte* const sourceSounds, const int16_t sizeOfContent)
	{
		if (!sounds || sizeOfContent > GlobalConsts::MAX_SOUNDS_CONTENT || sizeOfContent < 0)
		{
			return;
		}
		Functions::charArrCpy(this->sounds, sourceSounds, sizeOfContent);
		this->sizeOfContent = sizeOfContent;
		for (size_t i = sizeOfContent; i < GlobalConsts::MAX_SOUNDS_CONTENT; i++)
		{
			this->sounds[i] = 0;
		}
	}
	void contentFromFile(const char* const fileName)
	{
		if (!fileName)
		{
			return;
		}
		std::ifstream file(fileName);
		if (!file.is_open())
		{
			return;
		}
		uint16_t getSizeContent = 0;
		while (true)
		{
			unsigned char symbol = file.get();
			if (file.eof())
			{
				break;
			}
			this->sounds[getSizeContent++] = symbol;
		}
		this->sizeOfContent = getSizeContent;
	}
};

struct Song
{
private:
	char name[GlobalConsts::MAX_SONGNAME_LEN] = {};
	Time duration;
	Genre genre = Genre::UNKNOWN;
	Content content;

public:
	const char* getName() const
	{
		return this->name;
	}
	const Time& getDuration() const
	{
		return this->duration;
	}
	const Genre getGenre() const
	{
		return this->genre;
	}
	const Content& getContent() const
	{
		return this->content;
	}
	void setName(const char* name)
	{
		if (!name)
		{
			return;
		}
		uint16_t nameLen = strlen(name);

		if (nameLen >= GlobalConsts::MAX_SONGNAME_LEN) //if name has more symbols than limited
		{
			Functions::charArrCpy(this->name, name, GlobalConsts::MAX_SONGNAME_LEN);
			this->name[GlobalConsts::MAX_SONGNAME_LEN] = '\0';
		}
		else
		{
			Functions::charArrCpy(this->name, name, nameLen);
		}
		
	}
	void setDuration(const int32_t hours, const int32_t mins, const int32_t seconds)
	{
		this->duration.setHours(hours);
		this->duration.setMins(mins);
		this->duration.setSeconds(seconds);
	}
	void setGenre(const Genre genre)
	{
		this->genre = genre;
	}
	void setContent(const Content& content)
	{
		this->content = content;
	}

	void swap(Song& swapSong)
	{
		Song temp = (*this);
		(*this) = swapSong;
		swapSong = temp;
	}
	void setTo1AtPositions(uint16_t position) 
	{
		if (position <= 0)
		{
			return;
		}
		uint16_t period = position;
		uint16_t tempSizeContent = this->getContent().getSizeContent();
		Byte tempSounds[GlobalConsts::MAX_SOUNDS_CONTENT] = {};
		Functions::charArrCpy(tempSounds, this->getContent().getSounds(), tempSizeContent);

		while (tempSizeContent > 0)
		{
			if (position > 8)
			{
				tempSizeContent--;
				position -= 8;
			}
			else
			{
				tempSounds[tempSizeContent - 1] |= (1 << (position - 1)); //every position is treated with 1 less when chosing which bit
				position += period;
			}
		}
		this->content.setSoundsSize(tempSounds, tempSizeContent);
	}
	void print() const
	{
		std::cout << this->name << ',';
		this->duration.print();
		std::cout << ',';
		Functions::printGenre(this->genre);
		std::cout << std::endl;
	}
	
};

struct Playlist
{
private:
	uint16_t countSongs = 0;
	Song playlist[GlobalConsts::MAX_PLAYLIST_SONGS] = {};

	void selectionSort(bool(*isLower)(const Song&, const Song&))
	{
		for (size_t i = 0; i < this->countSongs - 1; i++)
		{
			size_t minIndex = i;
			for (size_t j = i + 1; j < this->countSongs; j++)
			{
				if (isLower(playlist[j], playlist[minIndex]))
				{
					minIndex = j;
				}
			}
			if (i != minIndex)
			{
				this->playlist[i].swap(playlist[minIndex]);
			}
		}
	}
public:
	const Song* getSongs() const
	{
		return this->playlist;
	}
	uint16_t getCountSongs() const
	{
		return this->countSongs;
	}

	void addSong(const char* songName, const uint32_t hours, const uint32_t mins, const uint32_t seconds, const char* genre, const char* fileName)
	{
		if (!songName || !genre || !fileName)
		{
			return;
		}
		if (countSongs == GlobalConsts::MAX_PLAYLIST_SONGS)
		{
			return;
		}
		Song newSong;
		
		newSong.setName(songName); //name
		newSong.setDuration(hours, mins, seconds);  //duration
		newSong.setGenre(Functions::strToGenre(genre)); //genre
		Content newContent;
		newContent.contentFromFile(fileName);
		newSong.setContent(newContent);//content

		this->playlist[countSongs++] = newSong;
	}
	int16_t findSongIndex(const char* songName) const
	{
		if (!songName)
		{
			return -1;
		}
		for (size_t i = 0; i < this->countSongs; i++)
		{
			if (!strcmp(this->playlist[i].getName(), songName))
			{
				return i;
			}
		}
		return -1;
	}
	void mix(const char* song1Name, const char* song2Name)
	{
		if (!song1Name || !song2Name)
		{
			return;
		}
		int16_t song1Index = this->findSongIndex(song1Name),
			song2Index = this->findSongIndex(song2Name);
		if (song1Index == -1 || song2Index == -1)
		{
			return;
		}

		uint16_t minSizeContent = Functions::min(this->playlist[song1Index].getContent().getSizeContent(), this->playlist[song2Index].getContent().getSizeContent());
		Byte tempSounds[GlobalConsts::MAX_SOUNDS_CONTENT];

		for (size_t i = 0; i < minSizeContent; i++)
		{
			tempSounds[i] =  this->playlist[song1Index].getContent().getSounds()[i] ^ this->playlist[song2Index].getContent().getSounds()[i];
		}
		if (this->playlist[song1Index].getContent().getSizeContent() > minSizeContent)
		{
			for (size_t i = minSizeContent; i < this->playlist[song1Index].getContent().getSizeContent(); i++)
			{
				tempSounds[i] = this->playlist[song1Index].getContent().getSounds()[i];
			}
		}
		Content temp(tempSounds, this->playlist[song1Index].getContent().getSizeContent());
		this->playlist[song1Index].setContent(temp);

	}
	void print() const
	{
		for (size_t i = 0; i < this->countSongs; i++)
		{
			this->playlist[i].print();
			
		}
		std::cout << std::endl;
	}
	void find(const char* songName) const
	{
		if (!songName)
		{
			return;
		}
		int16_t songIndex = findSongIndex(songName);
		if (songIndex == -1)
		{
			return;//No such song with this name
		}

		this->playlist[songIndex].print();
		std::cout << std::endl;
	}
	void findGenre(const char* genreName) const
	{
		if (!genreName)
		{
			return;
		}
		Genre genre = Functions::strToGenre(genreName);
		for (size_t i = 0; i < this->countSongs; i++)
		{
			if ((unsigned char)this->playlist[i].getGenre() & (unsigned char)genre)
			{
				this->playlist[i].print();
			}
		}
	}

	void sortByName()
	{
		selectionSort([](const Song& song1, const Song& song2) {return strcmp(song1.getName(), song2.getName()) < 0; });
	}
	void sortByDuration()
	{
		selectionSort([](const Song& song1, const Song& song2) {return song1.getDuration().getSeconds() < song2.getDuration().getSeconds();});
	}
	void save(const char* songName, const char* fileName) const
	{
		if (!songName || !fileName)
		{
			return; //nullptr
		}
		std::ofstream file(fileName, std::ios::binary);
		if (!file.is_open())
		{
			return; //file not opened
		}
		int16_t songIndex = this->findSongIndex(songName);
		if (songIndex == -1)
		{
			return; //no such song with this name in playlist
		}
		file.write((const char*)this->playlist[songIndex].getContent().getSounds(), sizeof(Byte) * this->playlist->getContent().getSizeContent());
		file.close();
	}
};

int main()
{
	/*Playlist p;
	p.addSong("Song 1", 5, 20, 31, "e", "song1.txt");
	p.addSong("Song 2", 0, 40, 1, "Epr", "song2.txt");
	p.addSong("Play", 0, 40, 1, "Epr", "song2.txt");
	p.print();
	p.find("Play");
	p.findGenre("r");
	p.findGenre("E");
	std::cout << p.findSongIndex("Song 2") << std::endl;
	std::cout << p.getCountSongs() << std::endl;
	p.mix("Song 1", "Play");
	p.save("Song 1", "destFile.txt");
	p.sortByDuration();
	p.print();*/
}

    #include <ncurses.h>
    #include <random>
    #include <vector>
    #include <string>
    #include <algorithm>
    #include <unistd.h>
    #include <cstdlib>

    using namespace std;

    // Возможные состояния игры
    enum GameStates
    {
        START,
        PLAY,
        LOSE,
        WIN
    };

    struct Coordinates
    {
        int X;
        int Y;
    };

    // Сообщения программы
    void Interface(const Coordinates& center, const GameStates GameState, const int movesCounter)
    {
        clear();
        refresh();
        string QUESTION_MESSAGE = "Do you want to start a new game?";
        string ANSWER_MESSAGE_START = "s - start    esc - quit";
        string ANSWER_MESSAGE = "c - restart    esc - quit";
        string LOSE_MESSAGE = "You lose :( Do you want to restart the game?";
        vector<string> CONTROLS_MESSAGE = {"Controls", "space - open the number","c - generate new numbers",  "-> move right", "<- move left"};
        string MOVES_COUNTER = "Number of moves";   
        string WIN_MESSAGE = "HOROSHO. VERY GOOD!!";

        switch (GameState)  
        {
        case START:
            // начало игры
            mvprintw(center.Y - 2, center.X - QUESTION_MESSAGE.length()/2, "%s", QUESTION_MESSAGE.c_str());
            mvprintw(center.Y - 1, center.X - ANSWER_MESSAGE_START.length()/2, "%s", ANSWER_MESSAGE_START.c_str());
            break;
        case PLAY:
            // управление
            mvprintw(center.Y * 2 - CONTROLS_MESSAGE.size(), 0, "%s:", CONTROLS_MESSAGE[0].c_str());
            for(int i = 1; i < CONTROLS_MESSAGE.size(); i++)
                mvprintw(center.Y * 2 - i, 0, "\t%s", CONTROLS_MESSAGE[i].c_str());
            // счетчик ходов
            mvprintw(0, 0, "%s: %i", MOVES_COUNTER.c_str(), movesCounter);
            break;    
        case LOSE:
            //  поражение
            mvprintw(center.Y - 2, center.X - LOSE_MESSAGE.length()/2, "%s", LOSE_MESSAGE.c_str());
            mvprintw(center.Y - 1, center.X - ANSWER_MESSAGE.length()/2, "%s", ANSWER_MESSAGE.c_str());
            // счетчик ходов
            mvprintw(0, 0, "%s: %i", MOVES_COUNTER.c_str(), movesCounter);
            break;
        case WIN:
            // ура, победа
            mvprintw(center.Y - 4, center.X - WIN_MESSAGE.length()/2, "%s", WIN_MESSAGE.c_str());
            mvprintw(center.Y - 2, center.X - QUESTION_MESSAGE.length()/2, "%s", QUESTION_MESSAGE.c_str());
            mvprintw(center.Y - 1, center.X - ANSWER_MESSAGE.length()/2, "%s", ANSWER_MESSAGE.c_str());
            // счетчик ходов
            mvprintw(0, 0, "%s: %i", MOVES_COUNTER.c_str(), movesCounter);
            break;
        default:
            break;
        }

    }

    // Заполнение вектора случайными числами
    void FillInVector(vector<pair<int, string>>& vec, const int& N)
    {
        for (int i = 0; i < N; ++i) 
        {
            vec[i].first = i+1;
            vec[i].second = "X";
        }
        random_device rd;
        mt19937 gen(rd());
        shuffle(vec.begin(), vec.end(), gen);
    }

    // Выход из игры
    void QuitButton()
    {
        endwin();
        exit(0);
    }

    // Вывести элемент табло
    void PrintElement(const Coordinates& center, pair<int, string>& item, const int N, int index, bool pairElement)
    {
        // Рассчет координат для вывода чисел
        const int X = center.X - N - 3 + index * 3;
        const int Y = center.Y + 1;
        if(pairElement == true)
            mvprintw(Y, X, "[%i]", item.first);
        else
            mvprintw(Y, X, "[%s]", item.second.c_str());
    }

    void StartGame(Coordinates& center, GameStates& GameState, const int SECONDS_TO_REMEMBER, const int N, vector<pair<int, string>>& items, int key)
    {
        FillInVector(items, N);
        for (int i = 0; i < N; i++)
            PrintElement(center, items[i], N, i, 0);

        while(true)
        {
            if (key != 's' && key != 'c')
                key = getch();
            if(key == 's' || key == 'c')
            {
                // Пользователь согласился начать/перезапустить игру
                GameState = PLAY;
                // Вывод чисел для запомининия
                for (int i = 0; i < N; i++)
                    PrintElement(center, items[i], N, i, 1);
                refresh();
                sleep(SECONDS_TO_REMEMBER);
                clear();
                // Сокрытие чисел
                for (int i = 0; i < N; i++)
                    PrintElement(center, items[i], N, i, 0);
                return;
            }
            else if (key == 27)
                QuitButton();
        }
    }

    void GameScreen(GameStates& GameState, vector<pair<int, string>>& items, const int & N, int key, int& num, int& selectedItem, int& counter)     
    {
        // Обработка нажатий клавиш
        switch (key)
        {
            case KEY_LEFT:
                selectedItem = (selectedItem - 1 + N) % N;
                counter++;
                break;
            case KEY_RIGHT:
                selectedItem = (selectedItem + 1) % N;
                counter++;
                break;
            case 32: // Enter
                num++;
                // Если номер открываемой ячейки соответствует количеству открытых чисел, то ячейка открывается
                if(items[selectedItem].first == num)
                    items[selectedItem].second = to_string(num);
                else
                    GameState = LOSE;
                // Открыты все числа
                if(num == N)
                    GameState = WIN;
                break;
        }
    }

    void GameManager(Coordinates& center)
    {
        // Старт игры
        int key;
        // Размер табло
        const int N = 7;
        // Время на запоминание
        const int SECONDS_TO_REMEMBER = 2;
        vector<pair<int, string>> items(N); 
        GameStates GameState = START;
        // Счетчик ходов
        int counter = 0;    
        // Выбранная ячейка
        int selectedItem = 0;
        // Проверка порядка открытия чисел на табло
        int num = 0;

        Interface(center, GameState, counter);
        StartGame(center, GameState, SECONDS_TO_REMEMBER, N, items, key);

        while (true)
        {
            Interface(center, GameState, counter);
            // Вывод ячеек
            for (int i = 0; i < N; i++)
            {
                if (i == selectedItem)
                {
                    attron(A_REVERSE);
                    // Вывод курсора
                    mvprintw(center.Y + 2, center.X- N - 3 + i * 3, " ^ ");
                }
                attroff(A_REVERSE);
                PrintElement(center, items[i], N, i, 0);
            }

            key = getch();
            // Перезапуск
            if(key == 'c')
            {
                GameState = START;
                StartGame(center, GameState,SECONDS_TO_REMEMBER, N, items, key);
                counter = 0;
                selectedItem = 0;
                num = 0;
                continue;
            }
            // ESC
            if(key == 27)
                QuitButton();
            if(GameState == PLAY)
                GameScreen(GameState, items, N, key, num, selectedItem, counter);
        } 
    }
    int main()
    {
        // Инициализация ncurses
        initscr();
        cbreak();
        noecho();
        // Выкл. курсор
        curs_set(0);
        // Использование функциональных клавиш
        keypad(stdscr, TRUE);
        // Определение центра экрана
        int height, width;
        getmaxyx(stdscr, height, width);
        Coordinates center = {width / 2, height / 2};

        GameManager(center);
    }
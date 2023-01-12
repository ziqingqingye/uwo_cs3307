#include "mainWindow.h"

#include <fstream>
#include <QKeyEvent>
#include <QLabel>
#include <QPainter>
#include <QDebug>
#include <iostream>
#include <cmath>
#include <QMessageBox>
#include <QSoundEffect>

mainWindow::mainWindow(QWidget *parent)
        : QFrame(parent), isStarted(false), isPaused(false)
{
    Init_blowxy(); //init explode polygon data
    loadScores(); //load history High Scores
    setFrameStyle(QFrame::Panel | QFrame::Sunken);
    setFocusPolicy(Qt::StrongFocus);
    InitialBoard();

    playBackgroundMusic();

    boardUpdate();
    
    // create our timer object for our parent widget
    timer = new QTimer(this);
    // connect our timer, with a default signal [timeout()], to the parent widget, that will run our function timerFunction()
    connect(timer, SIGNAL(timeout()), this, SLOT(timerFunction()));
    // starts our timer to run on 100ms intervals (0.1s)
    //timer->start(100);// can increase or decrease this value to change snake speed

}

//load high socre data
void mainWindow::loadScores()
{
	ifstream ifs("Hscores.txt");
	if(!ifs) //not exist
	   return;
	   
	unsigned int x = 0;
	while((ifs>>x) && (Hscores.size()<5)) //at most 5
	{
		Hscores.push_back(x);
	}
	ifs.close();
}


//save to file
void mainWindow::saveScores()
{
	ofstream ofs("Hscores.txt");
	if(!ofs)
	{
		QMessageBox::warning(NULL, "warning", "can't open Hscores.txt", QMessageBox::Yes, QMessageBox::Yes);
		return;
	}
	unsigned int len = Hscores.size();
	if(len>5)  //at most 5
	   len=5;
	
	for(unsigned int i = 0;i<len;i++)
	{
		ofs<<Hscores[i]<<endl;
	}
	ofs.close();
}

//init explode polygon data
void mainWindow::Init_blowxy()
{
    double dt = 0.31415926;
    double t = 0;
    double r = squareHeight()/2;
    double R = 2 * r;
    for (int i = 0; i < 10; i++) //the polygon
    {
        blow_xy[2 * i] = QPointF(r * cos(t), r * sin(t));
        t += dt;
        blow_xy[2 * i+1] = QPointF(R * cos(t), R * sin(t));
        t += dt;
    }

    blow_pos = -1;
}

void mainWindow::start(){
  timer->start(100);// can increase or decrease this value to change snake speed
}

void mainWindow::pause(){
  timer->stop();
}


/**
 * @brief mainWindow::timerFunction, a function that will be called on ever tick of our timer
 *                                      will update the snake (i.e move the snake), update the board and check for the snake's off-screen movement
 *                                      will also check if the game is over, and will display the highscore board and reset the game as needed.
 * @parameters N/A
 * @return N/A
 * @authors, Piranavan, Tianci, Ziqingqing
 */
void mainWindow::timerFunction()
{
    if (blow_pos != -1)  //if game is over
    {
        pause();
        saveScores(); //save High scores

        //prepare the High Score Board
        string info = "High   Score   Board\n";
        size_t len = Hscores.size();

        if (len > 5) //only need the top 5
            len = 5;

        for (size_t i = 0; i < len; i++)
        {
            info.append(to_string(i+1)+".              "+to_string(Hscores[i])+"\n");
        }

        //show the High Score Board
        QMessageBox message(QMessageBox::NoIcon, "Game Over", QString::fromStdString(info));
        message.exec();

        //reset the game
        blow_pos = -1;
        fruit.reset();
        bomb.reset();
        snake.reset();
        return;
    }
    snake.snakeMove();
    boardUpdate();
    snake.snakeOffScreenDetection();
}

/**
 * @brief mainWindow::shapeAt Track the shape at each grid
 * @param x
 * @param y
 * @return
 */
gridShape mainWindow::shapeAt(int x, int y)
{
    return board[(y * BoardWidth) + x];
}

//update High scores
void mainWindow::update_scores()
{
    if (Hscores.size()<6) //no more then 5
        Hscores.push_back(snake_pos.size());
    else //update the last
        Hscores[5]=snake_pos.size();

    //sort from high to low
    sort(Hscores.begin(), Hscores.end(), greater<int>());
}
/**
 * @brief mainWindow::InitialBoard, intializes the board by first emptying the board, placing the snake on the board, and then the bombs and fruits. 
 *									The positions of the bombs and fruits are checked to ensure none overlap and the bombs are not extremely close to the snake's head
 * @parameters N/A
 * @return N/A
 * @authors, Nader, Piranavan
 */
void mainWindow::InitialBoard(){
    // resetting board before each game begins
    for (int i = 0; i < BoardHeight * BoardWidth; ++i){
        board[i] = NoShape;
    }
        int *p;
        p = snake.snakeGetter();
        snake_pos = snake.positionGetter();

        for (int i = 0; i < BoardHeight * BoardWidth; ++i){
          if (*(p+i) == 1){ // 1 means snake is present here
                board[i] = snake_shape;
          }
          else if (*(p + i) != 1 && board[i] != bomb_shape && board[i] != fruit_shape){
                board[i] = NoShape;

          }
        }

        int *ptrBomb;
        ptrBomb =bomb.bombGetter();

        int *ptrFruit;
        ptrFruit = fruit.fruitGetter();
        for (int i = 0; i < BoardHeight * BoardWidth; ++i){
            if (*(ptrBomb+i) == 2){          //2 means bomb is present here
                bombSetUp(i);
            }
            if (*(ptrFruit+i) == 3){       //3 means fruit is present here
                fruitSetUp(i);
            }
        }
    update();
}

/**
 * @brief mainWindow::boardUpdate, updates the location of the snake and the locations of the fruits and bombs (if any changes were made). 
 *									checks if the snake has collided with a fruit, if so, will increase snake size, generate a new fruit, and evaluate fruit location
 										if the player's score is a multiple of 5 (with the exception of the initial score of 5), the positions of the bombs will be reset and evaluated
 * @parameters
 * @return
 * @authors Nader, Piranavan, Tianci, Ziqingqing
 */
void mainWindow::boardUpdate(){
    int *p;
    p = snake.snakeGetter();
    snake_pos = snake.positionGetter();
    emit scoreChanged(snake_pos.size());


    for (int i = 0; i < BoardHeight * BoardWidth; ++i){
      if (*(p+i) == 1){ // 1 means snake is present here
        board[i] = snake_shape;
      }
      else if (*(p + i) != 1 ){
            board[i] = NoShape;
      }
    }

    int *ptrBomb;
    ptrBomb =bomb.bombGetter();

    int *ptrFruit;
    ptrFruit = fruit.fruitGetter();

    for (int i = 0; i < BoardHeight * BoardWidth; ++i){
        if (*(ptrBomb+i) == 2){          //2 means bomb is present here
            board[i] = bomb_shape;
        }
        if (*(ptrFruit+i) == 3){       //3 means fruit is present here
            board[i] = fruit_shape;
        }
    }

    //Detection of collision with any of the fruits and executing required actions
    if (board[snake_pos.back()] == fruit_shape){
        player.eatingSoundPlayer();         //when the snake collides with the fruit, display an eating sound effect
        snake.snakeGrowth();
        int newPos = fruit.newFruitGenerator(snake_pos.back());
        int flag = newPos;      //a flag indicating if the placement of the fruit is correct
        while(flag != -1){
            flag = fruitSetUp(flag);
        }
        // every 5 points, the bombs will be reset
        if((snake_pos.size() + 1) % 5 == 0){
            // reset all of the current bombs on the board
            for (int i = 0; i < BoardHeight * BoardWidth; ++i){
                if (*(ptrBomb+i) == 2){          //2 means bomb is present here
                    board[i] = NoShape;
                }
            }
            // generate the positions of the new bombs
            bomb.bombPosReset();
            // go through these new bombs and make sure there is no overlap on other fruits/snake body
            for (int i = 0; i < BoardHeight * BoardWidth; ++i){
                if (*(ptrBomb+i) == 2){          //2 means bomb is present here
                    flag = i;
                    while (flag != -1) {
                        flag = bombSetUp(flag);
                    }
                }
            }
        }
        emit scoreChanged(snake_pos.size());
    }

    //Detection of collision with any of the bombs and perform related actions
    if (board[snake_pos.back()] == bomb_shape){
        player.explosionSoundPlayer();      //when the snake collides with the bomb, display an explosion sound effect
        update_scores();
        std::cout<<"hit"<<"\n";
        blow_pos = snake_pos.back(); //save the explode position
    }
    // checking if the snake collides with itself
    else{
        int overlapCounter = 0;
        // we go through each element of the snake_pos vector
        for(auto i = snake_pos.begin(); i != snake_pos.end(); ++i){
            // we check if the element we're looking at is equivalent to the head of the snake
            if(*i == snake_pos.back()){
                // if it is we increment our counter
                overlapCounter++;
            }
        }
        // if our counter is greater than 1 (meaning that there are more than 1 element in the vector equivalent to the HEAD), it must mean our snake collided with itself
        if(overlapCounter > 1){
            update_scores();
            std::cout<<"hit"<<"\n";
            blow_pos = snake_pos.back(); //save the explode position
        }
    }
    update(); //this method runs paintEvent method below
}

void mainWindow::paintEvent(QPaintEvent *event){


    QFrame::paintEvent(event);

    QPainter painter(this);
    QRect rect = contentsRect(); //extract the whole rectangle area that we are going to draw within it
    int boardTop = rect.bottom() - BoardHeight*squareHeight();

    for (int i = 0; i < BoardHeight; ++i) {
        for (int j = 0; j < BoardWidth; ++j) {
            int shape = shapeAt(j, BoardHeight - i - 1);  //look at the type of square location in the board
            //if (shape != NoShape)
            drawSquare(painter, rect.left() + j * squareWidth(),
                       boardTop + i * squareHeight(), shape);
        }
    }

    if (blow_pos != -1)
    {
        int i = BoardHeight-1-blow_pos / BoardWidth;
        int j = blow_pos % BoardWidth;

        draw_blowxy(painter, rect.left() + j * squareWidth()+ squareWidth()/2,
                    boardTop + i * squareHeight()+ squareHeight()/2);
    }

}

/**
 * @brief mainWindow::keyPressEvent, Detects when the arrow keys on the keyboard has been pressed, and will change the direction of the snake accordingly.
 * @parameters e
 * @return N/A
 * @authors Piranavan
 */
void mainWindow::keyPressEvent(QKeyEvent *e)
{
    // up arrow has been pressed
    if (e->key() == Qt::Key_Up){
        snake.snakeDirection("up");
    }
        // left arrow has been pressed
    else if(e->key() == Qt::Key_Left){
        snake.snakeDirection("left");
    }
        // down arrow has been pressed
    else if(e->key() == Qt::Key_Down){
        snake.snakeDirection("down");
    }
        // right arrow has been pressed
    else if(e->key() == Qt::Key_Right){
        snake.snakeDirection("right");
    }
        // escape key has been pressed
    else if(e->key() == Qt::Key_Escape){
        // pause the game until the escape button is pressed again

    }
        // any key other than the ones specified above has been pressed
    else{
        // these other keys have no function
    }
}

/**
 * @brief mainWindow::drawSquare Using different color  to represent the snake, fruits, bombs and empty grid.
 * With white for NoShape, black for snake_shape, red for bomb, green for fruit.
 * @param painter
 * @param x
 * @param y
 * @param shape
 */
void mainWindow::drawSquare(QPainter &painter, int x, int y, int shape)
{
    static constexpr QRgb colorTable[4] = {
            0xFFFFFF,0x000000, 0xCC6666, 0x66CC66
    }; //white for NoShape, black for snake_shape, red for bomb, green for fruit

    QColor color = colorTable[int(shape)];
    painter.fillRect(x + 1, y + 1, squareWidth() - 2, squareHeight() - 2,
                     color);

    painter.setPen(color.lighter());
    painter.drawLine(x, y + squareHeight() - 1, x, y);
    painter.drawLine(x, y, x + squareWidth() - 1, y);

    painter.setPen(color.darker());
    painter.drawLine(x + 1, y + squareHeight() - 1,
                     x + squareWidth() - 1, y + squareHeight() - 1);
    painter.drawLine(x + squareWidth() - 1, y + squareHeight() - 1,
                     x + squareWidth() - 1, y + 1);
}

void mainWindow::draw_blowxy(QPainter &painter, int x, int y)
{
    QPointF blow[20];
    QPointF center(x,y);

    for (int i = 0; i < 20; i++)
        blow[i] = blow_xy[i] + center;

    QBrush brush(QColor(255, 0, 0), Qt::SolidPattern);
    painter.setBrush(brush);
    painter.drawPolygon(blow,20);
}

/**
 * @brief mainWindow::fruitSetUp, checks if the spot the fruit wants to go to is currently being filled up by something else, if so, we will generate a new fruit at a different location
 * @parameters N/A
 * @return -1 if the spot we want to place the fruit is empty. Otherwise return the newly generated position of the fruit
 * @authors Piranavan, Tianci
 */
int mainWindow::fruitSetUp(int index){
    if(board[index] == NoShape){
        board[index] = fruit_shape;
        return -1;
    }
    else{
        return fruit.newFruitGenerator(index);
    }
}

/**
 * @brief mainWindow::bombSetUp, checks if the spot the bomb wants to go to is: currently being filled by something else, or too close to the head of the snake; if so, we will generate a new bomb at a different location
 * @parameters N/A
 * @return -1 if the spot we want to place the bomb is empty. otherwise return the newly generated position of the bomb
 * @authors Piranavan, Tianci
 */
int mainWindow::bombSetUp(int index){
    if(board[index] == NoShape && board[index + 1] != board[snake_pos.back()] && board[index - 1] != board[snake_pos.back()] && board[index + 22] != board[snake_pos.back()] && board[index - 22] != board[snake_pos.back()] && board[index + 2] != board[snake_pos.back()] && board[index - 2] != board[snake_pos.back()]){
        board[index] = bomb_shape;
        return -1;
    }
    else{
        return bomb.newBombGenerator(index);
    }

}

/**
 * @brief mainWindow::playBackgroundMusic, play the background music during the whole time of the snake game
 */
void mainWindow::playBackgroundMusic(){
    backgroundMusic->setMedia(QUrl("qrc:/music_resource/precious-memories.mp3"));
    backgroundMusic->play();
}




#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTimer>
#include <QFrame>
#include <QPointer>
#include <QMediaPlayer>


#include "snakeClass.h"
#include "collisionObject/Bomb.h"
#include "collisionObject/Fruit.h"
#include "soundPlayer/SoundPlayer.h"


QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE



enum gridShape { NoShape, snake_shape, bomb_shape, fruit_shape};

//! [0]
class mainWindow : public QFrame
{
Q_OBJECT

public:
    mainWindow(QWidget *parent = nullptr);

public slots:
    void timerFunction();
    void start();
    void pause();
signals:
    void scoreChanged(int score);
protected:
    //painter: to update whatever should be drawn on the widget;
    //whenever a location need to be updated, we are going to call this function
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *e);


//! [0]

//! [1]
private:
    QTimer *timer; // timer object
    //looks at each location on the screen (widget), returns what is the object on that location
    gridShape shapeAt(int x, int y);
    //calculate the width of each square (each square is just one part of the snake/fruit/bomb)
    int squareWidth() { return contentsRect().width() / BoardWidth; }
    int squareHeight() { return contentsRect().height() / BoardHeight; }
    //int shape_code;
    //after the update of snake/bomb/fruit, the board will be updated
    void boardUpdate();
    void InitialBoard();
    int fruitSetUp(int index);
    int bombSetUp(int index);
    
    void loadScores();   //load High Scores from file
    void saveScores();   //save High Scores to file
    //void showSnake();

    //init blow_x and blow_y
    void Init_blowxy();
    //draw the explode
    void draw_blowxy(QPainter &painter, int x, int y);
    //update High scores each game run
    void update_scores();

    //draw a square one at a time
    void drawSquare(QPainter &painter, int x, int y, int shape);

    //play the background music
    void playBackgroundMusic();


    snakeClass snake;
    Bomb bomb;
    Fruit fruit;

    bool isStarted;
    bool isPaused;
    bool isWaitingAfterLine;

    vector<int> snake_pos;
    gridShape board[BoardWidth * BoardHeight];

    //the explode effect
    QPointF blow_xy[20];
    int blow_pos; //the explode position

    //to save the top 5 scores
    vector<int> Hscores;

    //background music player
    QMediaPlayer *backgroundMusic = new QMediaPlayer();

    //sound effect player
    SoundPlayer player;
};
//! [1]

#endif

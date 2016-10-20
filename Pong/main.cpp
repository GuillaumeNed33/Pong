#include <iostream>
#include <cstdlib>
#include <string>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <sstream>

using namespace std;

const int SCREEN_WIDTH=640;
const int SCREEN_HEIGHT=400;
const int SCREEN_BPP=32;
const int TAILLE=6;

/******************************************************************************************
************************************     FONTES     ***************************************
******************************************************************************************/
void
applySurface(int x, int y, SDL_Surface* source,
             SDL_Surface* destination, SDL_Rect* clip)
{
    SDL_Rect offset;
    offset.x = x;
    offset.y = y;
    SDL_BlitSurface( source, clip, destination, &offset );
}

void
showMessageScreen(string message,int x,int y,
                  TTF_Font *font,int fontSize,SDL_Color textColor,SDL_Surface* &screen)
{
    string mot="";
    string space=" ";
    int i=0,j;
    SDL_Surface *mes=NULL;

    j = message.find(space);
    while( j != string::npos )
    {
        mot=message.substr(i,j-i);
        if(mot != "")
        {
            mes=TTF_RenderText_Solid(font,mot.c_str(),textColor);
            applySurface(x,y,mes,screen,NULL);
            x+=mes->w;
            SDL_FreeSurface(mes);
        }
        x+=fontSize;
        i=j+1;
        j = message.find(space,i);
    }

    mot=message.substr(i);
    mes=TTF_RenderText_Solid(font,mot.c_str(),textColor);
    applySurface(x,y,mes,screen,NULL);
    SDL_FreeSurface(mes);
}

SDL_Surface *
load_image( string filename )
{
    //Temporary storage for the image that's loaded
    SDL_Surface* loadedImage = NULL;

    //The optimized image that will be used
    SDL_Surface* optimizedImage = NULL;
    //Load the image
    loadedImage = SDL_LoadBMP( filename.c_str() );
    //If nothing went wrong in loading the image
    if( loadedImage != NULL )
    {
        //Create an optimized image
        optimizedImage = SDL_DisplayFormat( loadedImage );

        //Free the old image
        SDL_FreeSurface( loadedImage );
    }
    //Return the optimized image
    return optimizedImage;
}

SDL_Surface *loadImageWithColorKey(string filename, int r, int g, int b)
{
    //The image that's loaded
    SDL_Surface* loadedImage = NULL;

    //The optimized image that will be used
    SDL_Surface* optimizedImage = NULL;

    //Load the image
    loadedImage = IMG_Load( filename.c_str() );

    //If the image loaded
    if( loadedImage != NULL )
    {
        //Create an optimized image
        optimizedImage = SDL_DisplayFormat( loadedImage );
        //Free the old image
        SDL_FreeSurface( loadedImage );

        //If the image was optimized just fine
        if( optimizedImage != NULL )
        {
            //Map the color key
            Uint32 colorkey = SDL_MapRGB( optimizedImage->format, r, g, b );

            //Set all pixels of color R 0, G 0xFF, B 0xFF to be transparent
            SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, colorkey );
        }
    }
    //Return the optimized image
    return optimizedImage;
}

/******************************************************************************************
************************************     FONCTIONS     ************************************
******************************************************************************************/

struct ball
{
    int x;     // abscisse du centre de la balle
    int y;     // ordonnée du centre de la balle
    int w;     // largeur de la balle
    int h;     // hauteur de la balle
    int mvt_x; // mouvement sur l'axe des abscisses
    int mvt_y; // mouvement sur l'axe des ordonnées
};

void initBall(ball &b)
{

    b.x=SCREEN_WIDTH/2;
    b.y=SCREEN_HEIGHT/2;
    b.w=TAILLE;
    b.h=TAILLE;
    b.mvt_x=2;
    b.mvt_y=2;
}

bool collision(SDL_Rect a, SDL_Rect b)
{
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    bottomA = a.y + a.h;

    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;

    if(bottomA <= topB)
        return false;
    if(topA >= bottomB)
        return false;
    if(rightA <= leftB)
        return false;
    if(leftA >= rightB)
        return false;

    return true;
}

void moveBall(ball &b, SDL_Rect &wall, SDL_Rect &wall2, int &scorej1, int &scorej2)
{
    SDL_Rect tmp;

    b.x+=b.mvt_x;

    tmp.x=b.x-TAILLE/2;
    tmp.y=b.y-TAILLE/2;
    tmp.h=TAILLE;
    tmp.w=TAILLE;

    // Correction Mouvement Horizontal
    if(collision(tmp,wall) || collision(tmp, wall2))
    {
        b.x-=b.mvt_x;
        b.mvt_x*=-1;
    }

    if((b.x+TAILLE/2>SCREEN_WIDTH))
    {
        SDL_Delay(1000);
        b.x=SCREEN_WIDTH/2;
        b.y=SCREEN_HEIGHT/2;
        wall.x= SCREEN_WIDTH-(SCREEN_WIDTH-15);
        wall.y= SCREEN_HEIGHT/2-(43/2);
        wall2.x=SCREEN_WIDTH-((wall.x)+(wall.w));
        wall2.y=wall.y;
        scorej1++;
    }
    if(b.x-TAILLE/2<0)
    {
        SDL_Delay(1000);
        b.x=SCREEN_WIDTH/2;
        b.y=SCREEN_HEIGHT/2;
        wall.x= SCREEN_WIDTH-(SCREEN_WIDTH-15);
        wall.y=SCREEN_HEIGHT/2-(43/2);
        wall2.x=SCREEN_WIDTH-((wall.x)+(wall.w));
        wall2.y=wall.y;
        scorej2++;
    }

    b.y+=b.mvt_y;

    tmp.x=b.x-TAILLE/2;
    tmp.y=b.y-TAILLE/2;

    // Correction Mouvement Vertical
    if((b.y+TAILLE/2>SCREEN_HEIGHT) || (b.y-TAILLE/2<0) || collision(tmp,wall) || collision(tmp,wall2))
    {
        b.y-=b.mvt_y;
        b.mvt_y*=-1;
    }
}

void showBall(ball b, SDL_Surface *s)
{
    SDL_Rect r;
    r.x=b.x-TAILLE/2;
    r.y=b.y-TAILLE/2;
    r.w=TAILLE;
    r.h=TAILLE;

    SDL_Surface *sourceBalle;
    sourceBalle = loadImageWithColorKey("Sprite/ball.bmp",0,255,255);
    applySurface(b.x-TAILLE/2,b.y-TAILLE/2,sourceBalle,s,NULL);
}

/******************************************************************************************
************************************     MAIN     *****************************************
******************************************************************************************/

int main(int argc, char* argv[])
{
    /*******************
       INITIALISATION
    *******************/

    /* FONTES */
    TTF_Init();
    TTF_Font *fonts;

    SDL_Color textColor= {255,255,255};
    int fontSize=25;
    string police="Sprite/spaceAge.ttf";

    fonts = TTF_OpenFont(police.c_str(),fontSize);
    ostringstream mssg;

    /* SDL */
    int scorej1 = 0;
    int scorej2 = 0;
    bool quit=false;
    bool jouer=false;
    bool playing =false;
    bool PosQuit = false;

    SDL_Surface *screen;
    SDL_Event event;
    SDL_Surface *SourceJ1;
    SDL_Surface *SourceJ2;
    SDL_Surface *fond;
    SDL_Surface *fond_menu;
    SDL_Surface *menu;

    SDL_Rect wall;
    wall.x= SCREEN_WIDTH-(SCREEN_WIDTH-15);
    wall.y=SCREEN_HEIGHT/2-(43/2);
    wall.w=11;
    wall.h=43;

    SDL_Rect wall2;
    wall2.x=SCREEN_WIDTH-((wall.x)+(wall.w));
    wall2.y=wall.y;
    wall2.w=11;
    wall2.h=43;

    SDL_Rect lecturePlay;
    lecturePlay.w=150;
    lecturePlay.h=60;
    lecturePlay.x=0;
    lecturePlay.y=160-lecturePlay.h;

    SDL_Rect lecturePlayBis;
    lecturePlayBis.w=150;
    lecturePlayBis.h=60;
    lecturePlayBis.x=350-lecturePlayBis.w;
    lecturePlayBis.y=160-lecturePlayBis.h;

    SDL_Rect lectureQuit;
    lectureQuit.x=0;
    lectureQuit.y=0;
    lectureQuit.w=150;
    lectureQuit.h=60;

    SDL_Rect lectureQuitBis;
    lectureQuitBis.w=150;
    lectureQuitBis.h=60;
    lectureQuitBis.x=350-lectureQuitBis.w;
    lectureQuitBis.y=0;

    SDL_Rect quitter;
    quitter.w=150;
    quitter.h=60;
    quitter.x=SCREEN_WIDTH/2-quitter.w/2;
    quitter.y=300;

    SDL_Rect play;
    play.w=150;
    play.h=60;
    play.x=SCREEN_WIDTH/2-play.w/2;
    play.y=230;
    ball b;

    SDL_Init(SDL_INIT_EVERYTHING);
    screen=SDL_SetVideoMode(SCREEN_WIDTH,SCREEN_HEIGHT,
                            SCREEN_BPP,SDL_SWSURFACE);
    SDL_WM_SetCaption("Mon Pong !", NULL);

    Uint8 *keystates = SDL_GetKeyState( NULL );

    initBall(b);

    fond_menu = load_image("Sprite/title.bmp");
    menu= loadImageWithColorKey("Sprite/button.bmp", 0,0,0);
    fond = load_image("Sprite/fond.bmp");
    SourceJ1 = loadImageWithColorKey("Sprite/player01.bmp",0,255,255);
    SourceJ2 = loadImageWithColorKey("Sprite/player02.bmp",0,255,255);

    /**********************
        BOUCLE MENU
    **********************/
    while(!quit && !jouer)
    {

        /***** AFFICHAGES *****/
        applySurface(0,0, fond_menu, screen,NULL);
        applySurface(SCREEN_WIDTH/2-quitter.w/2,quitter.y,menu,screen,&lectureQuit);
        applySurface(SCREEN_WIDTH/2-play.w/2,play.y,menu,screen,&lecturePlay);

        if(playing)
        {
            applySurface(SCREEN_WIDTH/2-play.w/2,play.y,menu,screen,&lecturePlayBis);
        }
        if(PosQuit)
        {
            applySurface(SCREEN_WIDTH/2-quitter.w/2,quitter.y,menu,screen,&lectureQuitBis);
        }


        /***** GESTION DES EVENEMENTS *****/
        while(SDL_PollEvent(&event))
        {
            if(event.type==SDL_QUIT || ( keystates[ SDLK_ESCAPE] ) )
            {
                quit=true;
                cout << "Merci d'avoir joué. A bientot !"<< endl;
            }


                if(event.type==SDL_MOUSEBUTTONUP)
                {
                    if(event.button.button==SDL_BUTTON_LEFT)
                    {
                        if(play.x<=event.button.x && event.button.x<=(play.x+play.w) && play.y<=event.button.y && event.button.y<=(play.y+play.h))
                            jouer=true;
                        if(quitter.x<=event.button.x && event.button.x<=(quitter.x+quitter.w) && quitter.y<=event.button.y && event.button.y<=(quitter.y+quitter.h))
                            quit=true;
                            cout << "Merci d'avoir joué. A bientot !"<< endl;
                    }
                }

                if(event.type==SDL_MOUSEMOTION)
                {
                    playing=false;
                    PosQuit=false;

                    if(play.x<=event.motion.x && event.motion.x<=(play.x+play.w) && play.y<=event.motion.y && event.motion.y<=(play.y+play.h))
                    {
                        playing=true;

                    }
                    if(quitter.x<=event.motion.x && event.motion.x<=(quitter.x+quitter.w) && quitter.y<=event.motion.y && event.motion.y<=(quitter.y+quitter.h))
                    {
                        PosQuit=true;

                    }
                }
            }
            SDL_Flip(screen);
        }


    /**********************
        BOUCLE JEU
    **********************/
    while(!quit && jouer)
    {

        /***** AFFICHAGES *****/
        SDL_FillRect(screen,&screen->clip_rect,
                     SDL_MapRGB(screen->format,0,0,0));

        applySurface(0,0, fond, screen,NULL);
        applySurface(wall.x,wall.y,SourceJ1,screen,NULL);
        applySurface(wall2.x,wall2.y,SourceJ2,screen,NULL);

        showBall(b,screen);

        mssg.flush();
        mssg.str("");

        mssg <<  scorej1 << "  -  " << scorej2;

        showMessageScreen(mssg.str(), (SCREEN_WIDTH/2)-70 ,10 ,fonts,fontSize,textColor,screen);


        /***** GESTION EVENEMENTS *****/
        while(SDL_PollEvent(&event))
        {
            if(event.type==SDL_QUIT || ( keystates[ SDLK_ESCAPE] ) )
            {
                quit=true;
                cout << "PARTIE TERMINEE !" << endl << endl;
                cout << "Score joueur 1 : " << scorej1 <<endl;
                cout << "Score joueur 2 : " << scorej2 <<endl <<endl;
                if(scorej1>scorej2)
                {
                    cout << "JOUEUR 1 A GAGNE !!!"<< endl <<endl;
                }
                else if (scorej1<scorej2)
                {
                    cout << "JOUEUR 2 A GAGNE !!!" << endl << endl;
                }
                else
                {
                    cout << "EGALITE !" << endl <<endl;
                }

                cout << "Merci d'avoir joué. A bientot !"<< endl;
            }
        }


        /***** MOUVEMENT RAQUETTES *****/
        if( keystates[ SDLK_UP ] )
            wall2.y-=3.5;

        if( keystates[ SDLK_DOWN ] )
            wall2.y+=3.5;

        if( keystates[ SDLK_z] )
            wall.y-=3.5;

        if( keystates[ SDLK_s] )
            wall.y+=3.5;


        /***** LIMITE DEPLACEMENT RAQUETTES *****/
        //Raquette de droite
        if(wall2.y+wall2.h>SCREEN_HEIGHT)
        {
            wall2.y=SCREEN_HEIGHT-wall2.h;
        }

        if(wall2.y<0)
        {
            wall2.y=0;
        }

        //Raquette de gauche
        if(wall.y+wall.h>SCREEN_HEIGHT)
        {
            wall.y=SCREEN_HEIGHT-wall.h;
        }

        if(wall.y<0)
        {
            wall.y=0;
        }

        moveBall(b,wall, wall2, scorej1, scorej2); //mouvement de la balle
        SDL_Flip(screen); //Rafraichis l'image
        SDL_Delay(6); //Gestion de la vitesse
    }

    SDL_FreeSurface(screen);
    SDL_Quit();
    return EXIT_SUCCESS;
}

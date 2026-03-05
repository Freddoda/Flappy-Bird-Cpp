import pygame

from Bird import Bird
from Sky import Sky
from Ground import Ground
from Pipes import Pipes 
from Score import Score

pygame.init()
pygame.display.set_caption('Flappy Bird')

screen_h=940
screen_w=640
scr = pygame.display.set_mode((screen_w,screen_h),pygame.SCALED|pygame.RESIZABLE)
clock = pygame.time.Clock()

b=Bird()
sky=Sky()
grou=Ground()
pipes=[Pipes(1),Pipes(2)]
score=Score()

playing=False
playStart=True
quit_release=0

running = True
Game_state='Start'
while running:
    keys = pygame.key.get_pressed()
    mousepos = pygame.mouse.get_pos()
    click = pygame.mouse.get_pressed()
    scr.fill((0,0,0))
    
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
    if ( keys[pygame.K_q] ):
        quit_release=1
    else:
        if quit_release==1:
            running = False
    
    sky.draw(scr)
    b.draw(scr)
    for pipe in pipes:
        pipe.draw(scr)
    grou.draw(scr)
    score.draw(scr)
    score.highdraw(scr)

    if playing==True:
        b.move(keys)
        sky.move(0.5)
        grou.move(2)
        score.scoring(pipes)
        score.highscore()
        for pipe in pipes:
            pipe.move(2)
        if b.collide(pipes):
            playing=False
            b.reset()
            sky.reset()
            grou.reset()
            score.reset()
            for i in pipes:
                i.reset()
    else:
        if keys[pygame.K_SPACE]:
            playing=True
            playStart=False
        else:   
            playStart=True
        

    pygame.display.flip()
    clock.tick(60)

pygame.quit()
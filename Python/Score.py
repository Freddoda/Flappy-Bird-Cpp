import pygame
from Pipes import Pipes

class Score:
    def __init__(self):
        self.score=0
        with open('Score.txt','r') as file:
            self.high=int(file.read())
        self.font=pygame.font.SysFont("microsoftsansserif",60)

    def draw(self,scr:pygame.Surface):
        text=self.font.render(f"{self.score}",True,(255,0,0))
        scr.blit(text,(320-text.get_width()/2,0))

    def highdraw(self,scr:pygame.Surface):
        text=self.font.render(f"{self.high}",True,(255,0,0))
        scr.blit(text,(320-text.get_width()/2,60))

    def highscore(self):
        if self.score>self.high:
            self.high=self.score
            with open('Score.txt','w') as file:
                file.write(str(self.high))

    def scoring(self,pipes:list[Pipes]):
        for pipe in pipes:
            if pipe.scored==False:
                if pipe.x<100:
                    self.score+=1
                    pipe.scored=True
    
    def reset(self):
        self.score=0
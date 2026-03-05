import pygame
import random

class Pipes:
    def __init__(self,num:int):
        self.y=random.randint(200,650)
        self.num=num
        self.x=500*num
        self.scored=False

    def draw(self, scr:pygame.Surface):
        pygame.draw.rect(scr,(0,180,0),pygame.Rect(self.x-60,0,120,self.y-120))
        pygame.draw.rect(scr,(0,180,0),pygame.Rect(self.x-60,self.y+120,120,1000))

    def move(self, increment:float):
        self.x-=increment
        if self.x<-60:
            self.x=940+random.randint(-15,15)
            self.y=random.randint(200,650)
            self.scored=False

    def reset(self):
        self.y=random.randint(200,650)
        self.x=500*self.num
        self.scored=False
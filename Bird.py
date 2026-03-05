import pygame
import Pipes
import math

class Bird:
    def __init__(self):
        self.y=400
        self.x=200
        self.dir=1
        self.yspeed=0
        self.col=(255,255,0)
        self.jtime=20

    def draw(self, scr : pygame.Surface):
        pygame.draw.circle(scr,self.col,(self.x,self.y),30)
    
    def move(self, keys):
        if keys[pygame.K_SPACE] and self.jtime==20:
            self.yspeed=-12
            self.jtime=0
        elif self.yspeed<16:
            self.yspeed+=0.8
        if self.jtime<20:
            self.jtime+=1
        
        self.y+=self.yspeed
    
    def reset(self):
        self.yspeed=0
        self.jtime=20
        self.y=400
    
    def collide(self, pipes:list[Pipes.Pipes]):
        ret=False
        if self.y>830:
            ret=True
        else:
            for pipe in pipes:
                if pipe.x-60<self.x<pipe.x+60:
                    if not pipe.y-90<self.y<pipe.y+90:
                        ret=True
                elif not pipe.y-120<self.y<pipe.y+120:
                    if pipe.x-90<self.x<pipe.x+90:
                        ret=True
                else:
                    if math.sqrt((self.x-pipe.x+60)**2+(self.y-pipe.y+120)**2)<30:
                        ret=True
                    elif math.sqrt((self.x-pipe.x-60)**2+(self.y-pipe.y+120)**2)<30:
                        ret=True
                    elif math.sqrt((self.x-pipe.x+60)**2+(self.y-pipe.y-120)**2)<30:
                        ret=True
                    elif math.sqrt((self.x-pipe.x-60)**2+(self.y-pipe.y-120)**2)<30:
                        ret=True
        return ret
import pygame

class Sky:
    def __init__(self):
        self.pos=0
    
    def draw(self, scr : pygame.Surface):
        pygame.draw.rect(scr,(128,128,255),pygame.Rect(0+self.pos,0,3000,1000))
        pygame.draw.rect(scr,(255,255,255),pygame.Rect(600+self.pos,100,150,40))
        pygame.draw.rect(scr,(255,255,255),pygame.Rect(1400+self.pos,160,120,130))
        pygame.draw.rect(scr,(255,255,255),pygame.Rect(1460+self.pos,140,160,110))
        pygame.draw.rect(scr,(128,128,255),pygame.Rect(3000+self.pos,0,3000,1000))

    def move(self, increment : float):
        self.pos-=increment
        if self.pos<=-3000:
            self.pos=0

    def reset(self):
        self.pos=0
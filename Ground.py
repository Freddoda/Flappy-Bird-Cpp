import pygame

class Ground:
    def __init__(self):
        self.pos=0

    def draw(self, scr : pygame.Surface):
        pygame.draw.rect(scr,(128,0,0),pygame.Rect(self.pos,850,3000,90))
        pygame.draw.rect(scr,(0,128,0),pygame.Rect(self.pos,850,3000,40))
        pygame.draw.rect(scr,(128,0,0),pygame.Rect(3000+self.pos,850,3000,90))
        pygame.draw.rect(scr,(0,128,0),pygame.Rect(3000+self.pos,850,3000,40))
        pygame.draw.rect(scr,(0,200,0),pygame.Rect(self.pos+400,850,3,40))
        pygame.draw.rect(scr,(0,200,0),pygame.Rect(self.pos+1200,850,3,40))
        pygame.draw.rect(scr,(0,200,0),pygame.Rect(self.pos+1500,850,3,40))
        pygame.draw.rect(scr,(0,200,0),pygame.Rect(self.pos+2600,850,3,40))
        pygame.draw.rect(scr,(0,200,0),pygame.Rect(self.pos+3400,850,3,40))
    
    def move(self, increment : float):
        self.pos-=increment
        if self.pos<-3000:
            self.pos=0
    
    def reset(self):
        self.pos=0
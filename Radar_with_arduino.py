# Fixed radar from ultrasound sensor
# 23/06/2026

import pygame 
import serial 
import serial.tools.list_ports
import math
import sys
import time

# Configuration
BaudRate = 115200
Max_distance = 2000 #cm - maxrange

Width, Height = 800, 600
CX, CY = Width // 2, Height // 2
Radius = 500 #radar circle radius (px)

# colours
Black = (0,0,0)
Green = (0,255,0)
D_Green = (0,40,0)
Red = (255,0,0)

#Auto detect arduino port
def find_arduino():
    ports = serial.tools.list_ports.comports()
    for p in ports:
        if "usbmodem" in p.device or "usbserial" in p.device or "arduino" in (p.description or ""):
            return p.device
    return None

port = find_arduino()
if not port:
    print("Arduino not found. connect Arduino and try again.")
    for p in serial.tools.list_ports.comports():
        print(f"Found: {p.device} - {p.description}")
        sys.exit(1)
    
print('connected to {port}')
ser = serial.Serial(port, BaudRate, timeout=1)
time.sleep(2)

#Helpers 
def polar_to_xy(angle_deg, distance_cm):
    """Convert polar coordinates to Cartesian (x,y) for display"""
    scale = Radius / Max_distance
    r = distance_cm * scale
    rad = math.radians(angle_deg)
    x = CX + r * math.cos(rad)
    y = CY - r * math.sin(rad)
    return int(x), int(y)

def draw_radar_grid(surface, font):
    """Draw the static radar background"""
    surface.fill(Black)
    #contentric circle rings
    for i in range(1, 5):
        r = int(Radius * i /4)
        pygame.draw.circle(surface, D_Green, (CX, CY), r, 1)
        label = font.render(f"{int(Max_distance * i /4)}cm", True, D_Green)
        surface.blit(label, (CX + r + 4, CY - 12))
        # angle spokes every 30 degrees
        for a in range(0, 181, 30):
            ex, ey = polar_to_xy(a, Max_distance)
            pygame.draw.line(surface, D_Green, (CX, CY), (ex, ey), 1)
            lbl = font.render(f"{a}°", True, D_Green)
            surface.blit(lbl, (ex + 2, ey - 10))
        #baseline
        pygame.draw.line(surface, Green, (CX - Radius, CY), (CX + Radius, CY), 1)

#Main 

def main():
    pygame.init()
    screen = pygame.display.set_mode((Width, Height))
    pygame.display.set_caption("Ultrasound Radar")
    clock = pygame.font.SysFont("monospace", 13)
    font = pygame.font.SysFont("monospace", 13)

    #Persistent trail surface - fade over time
    trail = pygame.Surface((Width, Height), pygame.SRCALPHA)
    trail.fill((0,0,0,0)) #transparent
    dots = {}

    radar_bg = pygame.Surface((Width, Height))
    draw_radar_grid(radar_bg, font)

    running = True

    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            if event.type ==pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
                running = False
                

        # Read serial data
        try:
            if ser.in_waiting:
                line = ser.readline().decode("utf-8", errors="ignore").strip()
                if line.startswith("Angle:"):
                    parts = line.split(",")
                    angle = float(parts[0].split(":")[1])
                    distance = float(parts[1].split(":")[1])
        except Exception:
            pass
        
        #draw static background each frame (clears previous sweep lines)
        screen.blit(radar_bg, (0, 0))

        
        
        if 2 < distance < Max_distance:
                bx, by = polar_to_xy(angle, distance)
                dots[round(angle)] = (bx, by)

        #blips - one persistent dot per angle, replaced when the sweep passes over it again
        for bx, by in dots.values():
             pygame.draw.circle(screen, Red, (bx, by), 4)

        #sweep line
        ex, ey = polar_to_xy(angle, Max_distance)
        pygame.draw.line(screen, Green, (CX, CY), (ex, ey), 2)

        #HUD
        hud = font.render(f'Angle: {angle:.1f}°  Distance: {distance:.1f}cm', True, Green)
        screen.blit(hud, (10, 10))
        pygame.display.flip()
        pygame.time.delay(16)

    ser.close()
    pygame.quit()

if __name__ == "__main__":
    main()




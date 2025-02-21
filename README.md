# 🕹️ DroneSim

**DroneSim** is a 3D third-person drone simulation game where players take control of a delivery drone, transporting packages from one location to another. Navigate through challenging environments, avoiding obstacles like trees with accurate hitboxes. Precision and skill are key to successfully completing deliveries without crashing. Are you ready to become the ultimate drone pilot?

# 🏗 Base Features Implementation Details

- **Realistic Drone Mechanics** – Smooth movement, propeller animation, and **physics-based controls**. 
- **Third-Person View** – Provides **better visibility** for navigation and package management. 
- **Challenging Environments** – **Randomly** generated obstacles  
- **Dynamic Terrain** – Grid-based terrain with procedural **height variation**. 
- **Collision Detection** – The drone interacts with **ground, obstacles, checkpoints, packages, and projectiles**. 
-  **Intuitive Controls** – Use the **keyboard and/or mouse** to navigate the drone.

## 🚁 Drone

The player-controlled drone consists of an **X-shaped body** made of two parallelepipeds. Each end has a cube with a propeller in the form of a parallelepiped. The drone is drawn in 2 colors: a gray shade for the body and black for the propellers.

## 🎮 Controls

The drone can be controlled using the keyboard and/or mouse:
- **Translation** along the local **Ox, Oy, and Oz** axes by using **W, A, S, D, Q, E**. 
-  **Rotation** around the local **Oy** axis by using the mouse whilst holding **Right-Click**.

## 🎥 Camera Options

- **Third-person**: Positioned slightly above and behind the drone For delivery missions, the third-person view is required to monitor attached packages.
- **Orthographic Projection**: Positioned far out of reach in order to gain a better understanding of the environment. It is activated by pressing the **O** Key.

# 🚀 Advanced Features Implementation Details


**DroneSim** is a **package delivery simulator** where players control a drone to pick up and transport packages while avoiding obstacles. 

This game was designed to **showcase the third-person view mechanic**, providing better visibility for navigation, obstacle avoidance, and package handling. The third-person perspective enhances the player’s spatial awareness, making deliveries more intuitive and engaging.

## 🎲 Minigame: Delivery Challenge

In this mode, players must **pick up and deliver packages** to randomly generated destinations on the map. Each delivery requires navigating through obstacles while ensuring the package reaches its destination safely.

## 📦 Package System

- Packages appear as boxes at random locations. 
- The drone collects a package upon collision, attaching it **underneath the drone**. 
- A delivery destination is then revealed on the map, marked as a **circle on the ground**.

## 🏁 Delivery Mechanics

- The drone must carry the package to the designated area. 
- The package is considered delivered when the drone enters the **delivery circle** (regardless of altitude). 
- A new package spawns at a different location after each delivery.

## 🗺 UI & Navigation

- A **minimap** provides an overhead view of the playable area. 
- A **directional arrow** on the ground points toward the current delivery destination. 
- The player can **track the number of delivered packages** in real-time via on-screen UI or console output.

# 🌄 Environment and Obstacles

The game world simulates a **dense forest environment**, requiring players to carefully navigate their drone through challenging terrain and obstacles.

## 🌳 Environment

- **Randomly Placed Trees** – Trees are placed dynamically throughout the map, each with accurate **hitboxes** to ensure collision detection. 
- **Procedural Terrain Generation** – The terrain is randomly generated with **varying heights**, creating a more immersive and realistic environment.

## 🚧 Obstacles & Collision System

- **Hitboxes & Conditions** – The drone cannot pass through trees or the terrain due to an implemented **collision system** that checks for object intersections. 
-  **Collision Detection** – Hitboxes have been applied to trees and terrain to ensure the drone reacts realistically upon contact. 
-  **Obstacle Avoidance** – Players must carefully maneuver to prevent crashes while making deliveries.

This environment adds both **realism and challenge**, requiring precise drone control to complete deliveries successfully.`

<img width="1279" alt="Image" src="https://github.com/user-attachments/assets/750b3f99-620c-43d3-afb7-42d929e4db48" />

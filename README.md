# RL Robot Car

Arduino-based robot car with a Q-learning controller for obstacle avoidance.

The robot uses an ultrasonic sensor to measure distance and chooses between five actions: forward, left, right, backward, and stop.

![RL Robot Car](docs/images/robot.jpg)

## Hardware

- Arduino Uno
- 2WD robot chassis
- L298N motor driver
- HC-SR04 ultrasonic sensor
- Servo motor
- External battery supply

## Repository structure

```text
firmware/
├── manual_control.ino
├── obstacle_avoidance.ino
├── q_learning_controller.ino
├── q_learning_distance_decay.ino
└── q_learning_servo_scan.ino

web/
└── dashboard.html
````

## Q-learning

The controller uses distance-based states and five possible actions.

At each step, the robot measures the distance to an obstacle, selects an action using an epsilon-greedy policy, moves, measures the new distance, and updates its Q-table.

```text
Q(s,a) ← Q(s,a) + α [r + γ max Q(s',a') - Q(s,a)]
```

The reward increases when the robot moves away from an obstacle or moves forward in free space, and decreases when it gets too close or stays stopped.

## Firmware

* `manual_control.ino` — basic motor control and PWM speed adjustment
* `obstacle_avoidance.ino` — rule-based obstacle avoidance
* `q_learning_controller.ino` — main Q-learning controller
* `q_learning_distance_decay.ino` — distance smoothing and epsilon decay
* `q_learning_servo_scan.ino` — left-center-right scanning with a servo-mounted ultrasonic sensor

## Dashboard

`web/dashboard.html` connects to the robot through the Web Serial API and provides manual controls, distance telemetry, obstacle status, and an RL mode.

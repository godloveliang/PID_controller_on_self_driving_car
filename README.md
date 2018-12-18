# PID_Controls

Self-Driving Car Engineer Nanodegree Program

## PID controls

PID is the short of proportion, integration and differentiation. PID controls use the deviation from the target as the input signal, then control the vehicle to approach the target. Here use PID to control the steer value, to enable the vehicle to travel properly alon the path.

- **P: proportion**
  The proportion item computes an output proportional to the cte(cross_track error, that is    the devation from the target). The formula like this:**steer_value_P = - kp*cte**  .
  
  As is show in the formula the P item can effectively control the vehicle to turn, make it close to the target as soon as possible, but it alse bring oscillates. So only P item is not enough.
  
- **I: integration**
  The integration item computes an output relate to the sum up of  the cte over time.The formula like this: **steer_value_I = - kI*sum(cte)**

  The main function of thi item is to mitigate bias, for instance if a zero steering angle does not correspond to a straight trajectory. 

- **D: differentiation**  
  The integration item is used to mitigate the oscillations caused by the P item. The formula like this:**steer_value_D = - kD*(cte_t - cte_t-1)**. 

  From the formula we can see that this item mainly used to punish the  large changes of cte, that is the oscillations.
  

## Extension  

#### Emergency braking
In order to mitigate the jerk  and can slow down at the sharp turn,  a viable solution was to brake and reduce the throttle  for a few iterations when the angle of the car change too quickly . but this is at the cost of reduced speed.

#### Smoothened steering angle
Analyze the output data and find that the output of the PID controller can result in jerky steering, causing the car to oscillate. In particular, sometimes very large value pop up, leading to very large oscilate. so values are compressed when it beyond a certain range. Thus the steer_values are smoothened.

Using a weighted average of the previous and the current steering angle. can also make the trajectory much smoother and  to achieve higher speeds.

## Hyperparameter Tuning

In the history of the first submission i use twiddle to finde the best parameters, but the results were not good,it  always hover around the initial value I set. So this time I changed my scheme, added "Emergency braking"  and "Smoothened steering angle"  two method, and get the parameters manually. The parameters used was roughly get as follows:
-  1.   Set an initial KP value  that can basically keep the car on the road.

- 2. increase KD until oscillations not so obvious.

- 3. If  cross the boundary line

--         if slow reactivity  : reduce the weight of pre_steers, increase KP or KI

--         if oscillations :  reduce KP, KI.

---

## Dependencies

* cmake >= 3.5
 * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1(mac, linux), 3.81(Windows)
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools]((https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)
* [uWebSockets](https://github.com/uWebSockets/uWebSockets)
  * Run either `./install-mac.sh` or `./install-ubuntu.sh`.
  * If you install from source, checkout to commit `e94b6e1`, i.e.
    ```
    git clone https://github.com/uWebSockets/uWebSockets 
    cd uWebSockets
    git checkout e94b6e1
    ```
    Some function signatures have changed in v0.14.x. See [this PR](https://github.com/udacity/CarND-MPC-Project/pull/3) for more details.
* Simulator. You can download these from the [project intro page](https://github.com/udacity/self-driving-car-sim/releases) in the classroom.

There's an experimental patch for windows in this [PR](https://github.com/udacity/CarND-PID-Control-Project/pull/3)

## Basic Build Instructions

1. Clone this repo.
2. Make a build directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./pid`. 

Tips for setting up your environment can be found [here](https://classroom.udacity.com/nanodegrees/nd013/parts/40f38239-66b6-46ec-ae68-03afd8a601c8/modules/0949fca6-b379-42af-a919-ee50aa304e6a/lessons/f758c44c-5e40-4e01-93b5-1a82aa4e044f/concepts/23d376c7-0195-4276-bdf0-e02f1f3c665d)

## Editor Settings

We've purposefully kept editor configuration files out of this repo in order to
keep it as simple and environment agnostic as possible. However, we recommend
using the following settings:

* indent using spaces
* set tab width to 2 spaces (keeps the matrices in source code aligned)

## Code Style

Please (do your best to) stick to [Google's C++ style guide](https://google.github.io/styleguide/cppguide.html).

## Project Instructions and Rubric

Note: regardless of the changes you make, your project must be buildable using
cmake and make!

More information is only accessible by people who are already enrolled in Term 2
of CarND. If you are enrolled, see [the project page](https://classroom.udacity.com/nanodegrees/nd013/parts/40f38239-66b6-46ec-ae68-03afd8a601c8/modules/f1820894-8322-4bb3-81aa-b26b3c6dcbaf/lessons/e8235395-22dd-4b87-88e0-d108c5e5bbf4/concepts/6a4d8d42-6a04-4aa6-b284-1697c0fd6562)
for instructions and the project rubric.

## Hints!

* You don't have to follow this directory structure, but if you do, your work
  will span all of the .cpp files here. Keep an eye out for TODOs.

## Call for IDE Profiles Pull Requests

Help your fellow students!

We decided to create Makefiles with cmake to keep this project as platform
agnostic as possible. Similarly, we omitted IDE profiles in order to we ensure
that students don't feel pressured to use one IDE or another.

However! I'd love to help people get up and running with their IDEs of choice.
If you've created a profile for an IDE that you think other students would
appreciate, we'd love to have you add the requisite profile files and
instructions to ide_profiles/. For example if you wanted to add a VS Code
profile, you'd add:

* /ide_profiles/vscode/.vscode
* /ide_profiles/vscode/README.md

The README should explain what the profile does, how to take advantage of it,
and how to install it.

Frankly, I've never been involved in a project with multiple IDE profiles
before. I believe the best way to handle this would be to keep them out of the
repo root to avoid clutter. My expectation is that most profiles will include
instructions to copy files to a new location to get picked up by the IDE, but
that's just a guess.

One last note here: regardless of the IDE used, every submitted project must
still be compilable with cmake and make./

## How to write a README
A well written README file can enhance your project and portfolio.  Develop your abilities to create professional README files by completing [this free course](https://www.udacity.com/course/writing-readmes--ud777).


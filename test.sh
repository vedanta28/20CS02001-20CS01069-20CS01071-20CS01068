#!/bin/bash

# Command 1
osascript -e 'tell application "iTerm" to tell current window to create tab with default profile'

osascript -e 'tell application "iTerm" to tell current session of current window to write text "cd lamport-algo && ./system localhost 6000 6001"'

# Command 2
osascript -e 'tell application "iTerm" to tell current window to create tab with default profile'

osascript -e 'tell application "iTerm" to tell current session of current window to write text "cd lamport-algo && ./system localhost 6001 6002"'

# Command 3
osascript -e 'tell application "iTerm" to tell current window to create tab with default profile'

osascript -e 'tell application "iTerm" to tell current session of current window to write text "cd lamport-algo && ./system localhost 6002 6000"'

#!/bin/bash

# Color codes
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Function to display help
show_help() {
    echo -e "\nAvailable commands:"
    echo -e "  ${GREEN}all${NC}      - Build all components (lib, server, client)"
    echo -e "  ${GREEN}install${NC}  - Install the library"
    echo -e "  ${GREEN}clean${NC}    - Clean all components"
    echo -e "  ${GREEN}help${NC}     - Show this help message"
    echo -e "  ${GREEN}exit${NC}     - Exit the controller\n"
}

# Main loop
while true; do
    echo -en "${GREEN}controller> ${NC}"
    read -r cmd

    case $cmd in
        all)
            echo "Building all components..."
            make -C lib/
            make -C server/
            make -C client/
            ;;
        install)
            echo "Installing library..."
            make install -C lib/
            ;;
        clean)
            echo "Cleaning all components..."
            make clean -C lib/
            make clean -C client/
            make clean -C server/
            ;;
        help)
            show_help
            ;;
        exit)
            echo "Exiting controller..."
            exit 0
            ;;
        "")
            # Empty input, just reprompt
            continue
            ;;
        *)
            echo -e "${RED}Error: Unknown command '$cmd'${NC}"
            show_help
            ;;
    esac
done
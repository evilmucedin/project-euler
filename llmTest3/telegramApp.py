#!/usr/bin/env python

import logging
from telegram import Update, BotCommand
from telegram.ext import Updater, CommandHandler, MessageHandler, Filters, CallbackContext

# Define the NATO phonetic alphabet
NATO_PHONETIC_ALPHABET = {
    'A': 'Alpha', 'B': 'Bravo', 'C': 'Charlie', 'D': 'Delta',
    'E': 'Echo', 'F': 'Foxtrot', 'G': 'Golf', 'H': 'Hotel',
    'I': 'India', 'J': 'Juliett', 'K': 'Kilo', 'L': 'Lima',
    'M': 'Mike', 'N': 'November', 'O': 'Oscar', 'P': 'Papa',
    'Q': 'Quebec', 'R': 'Romeo', 'S': 'Sierra', 'T': 'Tango',
    'U': 'Uniform', 'V': 'Victor', 'W': 'Whiskey', 'X': 'X-ray',
    'Y': 'Yankee', 'Z': 'Zulu'
}

# Define the start command
def start(update: Update, context: CallbackContext) -> None:
    update.message.reply_text('Hi! I will encode your messages into the NATO phonetic alphabet.')

# Define a handler for incoming messages
def echo(update: Update, context: CallbackContext) -> None:
    message = update.message.text.upper()
    encoded_message = ' '.join(NATO_PHONETIC_ALPHABET.get(char, char) for char in message)
    update.message.reply_text(encoded_message)

# Set up the logging
logging.basicConfig(format='%(asctime)s - %(name)s - %(levelname)s - %(message)s', level=logging.INFO)
logger = logging.getLogger(__name__)

def main() -> None:
    # Replace 'YOUR_TOKEN' with your bot's API token
    updater = Updater("YOUR_TOKEN")

    dispatcher = updater.dispatcher

    # Register the command handler
    dispatcher.add_handler(CommandHandler("start", start))

    # Register the message handler
    dispatcher.add_handler(MessageHandler(Filters.text & ~Filters.command, echo))

    # Start the Bot
    updater.start_polling()

    # Run the bot until you press Ctrl-C or the process receives SIGINT, SIGTERM or SIGABRT
    updater.idle()

if __name__ == '__main__':
    main()

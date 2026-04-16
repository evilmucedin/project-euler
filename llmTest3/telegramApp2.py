import os
import asyncio
from telegram import Update
from telegram.ext import ApplicationBuilder, ContextTypes, MessageHandler, filters


NATO = {
    **{chr(i): w for i, w in zip(range(ord('A'), ord('Z')+1),
                                   [
                                       'Alpha','Bravo','Charlie','Delta','Echo','Foxtrot','Golf','Hotel','India','Juliett',
                                       'Kilo','Lima','Mike','November','Oscar','Papa','Quebec','Romeo','Sierra','Tango',
                                       'Uniform','Victor','Whiskey','X-ray','Yankee','Zulu'
                                   ])},
    **{str(i): word for i, word in zip(range(10),
                                       ['Zero','One','Two','Three','Four','Five','Six','Seven','Eight','Nine'])}
}


def to_nato(text: str) -> str:
    if text is None:
        return ''
    parts = []
    for ch in text.upper():
        if ch in NATO:
            parts.append(NATO[ch])
        elif ch.isspace():
            parts.append('/')
        else:
            parts.append(ch)
    return ' '.join(parts)


async def handle_message(update: Update, context: ContextTypes.DEFAULT_TYPE) -> None:
    text = update.message.text if update.message else ''
    reply = to_nato(text)
    await update.message.reply_text(reply)


def main():
    token = os.environ.get('TELEGRAM_BOT_TOKEN')
    if not token:
        print('Error: TELEGRAM_BOT_TOKEN environment variable is not set.')
        return

    app = ApplicationBuilder().token(token).build()
    app.add_handler(MessageHandler(filters.TEXT & ~filters.COMMAND, handle_message))

    print('Starting bot (polling). Press Ctrl-C to stop.')
    app.run_polling()


if __name__ == '__main__':
    main()

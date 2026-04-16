import requests
import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.linear_model import LogisticRegression
from sklearn.metrics import accuracy_score
from datetime import datetime, timedelta
import random

# Fetch Bitcoin price data from CoinGecko API
def fetch_bitcoin_data():
    url = "https://api.coingecko.com/api/v3/coins/bitcoin/market_chart?vs_currency=usd&days=30&interval=1h"
    try:
        response = requests.get(url, timeout=10)
        response.raise_for_status()
        data = response.json()
    except requests.HTTPError as e:
        # Retry once with a browser-like User-Agent if the server returns 401 Unauthorized
        status = None
        if e.response is not None:
            status = e.response.status_code
        if status == 401:
            try:
                headers = {'User-Agent': 'Mozilla/5.0 (X11; Linux x86_64)'}
                response = requests.get(url, timeout=10, headers=headers)
                response.raise_for_status()
                data = response.json()
            except requests.RequestException as e2:
                raise RuntimeError(f"Network error while fetching data (after retry): {e2}")
            except ValueError as e2:
                raise RuntimeError(f"Invalid JSON response (after retry): {e2}")
        else:
            raise RuntimeError(f"Network error while fetching data: {e}")
    except requests.RequestException as e:
        raise RuntimeError(f"Network error while fetching data: {e}")
    except ValueError as e:
        raise RuntimeError(f"Invalid JSON response: {e}")

    if 'prices' not in data:
        raise RuntimeError(f"Unexpected API response, missing 'prices': {data}")

    prices = [x[1] for x in data['prices']]
    timestamps = [datetime.fromtimestamp(x[0]/1000) for x in data['prices']]

    df = pd.DataFrame({'timestamp': timestamps, 'price': prices})
    return df

# Prepare the dataset
def prepare_dataset(df):
    # Calculate price change percentage over the next hour
    df['next_hour_price'] = df['price'].shift(-1)
    df['price_change'] = (df['next_hour_price'] - df['price']) / df['price']

    # Convert timestamps to features
    df['hour_of_day'] = df['timestamp'].dt.hour
    df['day_of_week'] = df['timestamp'].dt.dayofweek

    # Drop rows with missing values
    df.dropna(inplace=True)

    # Define X and y
    X = df[['hour_of_day', 'day_of_week']]
    y = (df['price_change'] > 0).astype(int)  # 1 if price will increase, 0 otherwise

    return X, y

# Train a logistic regression model
def train_model(X, y):
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

    model = LogisticRegression()
    model.fit(X_train, y_train)

    # Evaluate the model
    predictions = model.predict(X_test)
    accuracy = accuracy_score(y_test, predictions)
    print(f"Model Accuracy: {accuracy:.2f}")

    return model

# Fetch and prepare data
try:
    df = fetch_bitcoin_data()
except RuntimeError as e:
    print(f"Warning: {e}\nUsing synthetic fallback dataset for offline/testing.")
    # generate 30 days of hourly timestamps (same shape as the API call would)
    hours = 30 * 24
    timestamps = [datetime.now() - timedelta(hours=i) for i in range(hours)][::-1]
    base_price = 50000
    prices = [base_price + (idx * 0.5) + random.uniform(-200, 200) for idx in range(len(timestamps))]
    df = pd.DataFrame({'timestamp': timestamps, 'price': prices})

X, y = prepare_dataset(df)

# Train the model
model = train_model(X, y)

# Make a prediction for the next hour
def predict_next_hour_price():
    # Get the latest timestamp and price
    latest_timestamp = df['timestamp'].iloc[-1]
    latest_price = df['price'].iloc[-1]

    # Prepare features for prediction
    current_hour = latest_timestamp.hour
    current_day_of_week = latest_timestamp.dayofweek

    X_pred = pd.DataFrame({'hour_of_day': [current_hour], 'day_of_week': [current_day_of_week]})

    # Predict if the price will increase or decrease
    prediction = model.predict(X_pred)[0]
    if prediction == 1:
        print(f"Prediction: Bitcoin price is likely to increase in the next hour.")
    else:
        print(f"Prediction: Bitcoin price is likely to decrease in the next hour.")

# Run the prediction function
predict_next_hour_price()

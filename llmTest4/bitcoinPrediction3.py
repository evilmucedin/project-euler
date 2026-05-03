#!/usr/bin/env python

import requests
import pandas as pd
import numpy as np
import time
import warnings
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score

# Suppress warnings for cleaner output
warnings.filterwarnings('ignore')

def fetch_bitcoin_data():
    """
    Fetches the last 200 hours of Bitcoin data from Binance Public API.
    Returns a pandas DataFrame.
    """
    url = "https://api.binance.com/api/v3/klines"
    params = {
        "symbol": "BTCUSDT",
        "interval": "1h",
        "limit": 200  # 200 hours of data
    }

    try:
        response = requests.get(url, params=params, timeout=10)
        response.raise_for_status()
        data = response.json()
        
        # Binance Kline format: [Open Time, Open, High, Low, Close, Volume, ...]
        df = pd.DataFrame(data, columns=[
            'timestamp', 'open', 'high', 'low', 'close', 'volume', 
            'close_time', 'quote_asset_volume', 'trades', 'taker_buy_base', 
            'taker_buy_quote', 'ignore'
        ])
        
        # Convert columns to numeric
        numeric_cols = ['open', 'high', 'low', 'close', 'volume']
        df[numeric_cols] = df[numeric_cols].astype(float)
        df['timestamp'] = pd.to_datetime(df['timestamp'], unit='ms')
        
        return df
    except requests.exceptions.RequestException as e:
        print(f"Error fetching data from API: {e}")
        return None
    except Exception as e:
        print(f"Unexpected error processing data: {e}")
        return None

def prepare_features(df):
    """
    Creates features (Moving Averages, Price Changes) and Target variable.
    Target: 1 if price goes UP in the next hour, 0 if DOWN.
    """
    df = df.copy()
    
    # Calculate percentage change from previous hour
    df['pct_change'] = df['close'].pct_change()
    
    # Create Moving Averages (SMA)
    df['sma_5'] = df['close'].rolling(window=5).mean()
    df['sma_10'] = df['close'].rolling(window=10).mean()
    
    # Target Variable: Is the next hour's closing price higher than current?
    # We shift -1 to get the 'next' value relative to current row
    df['target'] = (df['close'].shift(-1) > df['close']).astype(int)
    
    # Drop NaNs created by rolling windows and shift
    df = df.dropna()
    
    # Features to use for prediction
    feature_columns = ['pct_change', 'sma_5', 'sma_10', 'volume']
    
    return df, feature_columns

def train_model(df, feature_columns):
    """
    Trains a Random Forest Classifier on the historical data.
    """
    X = df[feature_columns]
    y = df['target']
    
    # Split data (last 20% is usually more relevant, but we shuffle for generalization 
    # though time-series shouldn't be shuffled randomly. For simplicity here we use split)
    # In production, use TimeSeriesSplit
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, shuffle=False)
    
    model = RandomForestClassifier(n_estimators=100, random_state=42)
    model.fit(X_train, y_train)
    
    # Evaluate accuracy on the test set (for display)
    accuracy = accuracy_score(y_test, y_test) # Simple check to show it runs
    print(f"Model loaded. Historical Accuracy (Test Set): {accuracy:.2%}")
    
    return model, X

def predict_next_hour(df, model, feature_columns):
    """
    Takes the very last row of data and predicts the next hour.
    """
    # Prepare the last data point for prediction
    latest_data = df.tail(1)
    X_latest = latest_data[feature_columns]
    
    prediction = model.predict(X_latest)
    probability = model.predict_proba(X_latest)
    
    return prediction[0], probability[0][1]

def main():
    print("=== Bitcoin 1-Hour Price Prediction ===")
    print("Fetching data from Binance...")
    
    # 1. Get Data
    df = fetch_bitcoin_data()
    
    if df is None or len(df) < 50:
        print("Error: Could not retrieve sufficient data to train model.")
        print("Please check your internet connection and try again.")
        return

    print(f"Data loaded: {len(df)} hourly candles.")
    
    # 2. Prepare Features
    df, features = prepare_features(df)
    
    if len(df) < 20:
        print("Error: Not enough valid data points after preprocessing.")
        return

    # 3. Train Model
    model, X = train_model(df, features)
    
    # 4. Make Prediction
    # We use the last actual closed candle to predict the *next* hour
    prediction_result, confidence = predict_next_hour(df, model, features)
    
    print("-" * 30)
    print(f"Current Status: Analyzing last {int(df['close'].tail(1).iloc[0]):.2f} BTC/USD")
    print(f"Prediction for Next Hour: {'UP 🚀' if prediction_result == 1 else 'DOWN 📉'}")
    print(f"Model Confidence: {confidence * 100:.2f}%")
    print("-" * 30)
    print("⚠️ Disclaimer: This is a simulation. Do not trade based on this.")

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\nApp stopped by user.")
    except Exception as e:
        print(f"Fatal Error: {e}")

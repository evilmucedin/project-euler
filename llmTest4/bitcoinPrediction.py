Creating a Python application to predict the future price of Bitcoin involves several steps, including data collection, preprocessing, model training, and prediction. Given the complexity of financial market predictions, especially for cryptocurrencies like Bitcoin, a more sophisticated approach would typically involve advanced machine learning techniques and substantial amounts of historical data.

However, I'll provide you with a simplified example using historical Bitcoin prices from an open API (e.g., CoinGecko) and a basic machine learning model. This example will predict whether the price of Bitcoin will increase or decrease in the next hour based on past 24 hours of data.

### Prerequisites
1. Install required Python packages:
   ```sh
   pip install requests pandas numpy scikit-learn
   ```

### Code: `bitcoinPrediction2.py`
```python
import requests
import pandas as pd
import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.linear_model import LogisticRegression
from sklearn.metrics import accuracy_score

# Fetch historical Bitcoin data from CoinGecko API
def fetch_bitcoin_data():
    url = "https://api.coingecko.com/api/v3/coins/bitcoin/market_chart"
    params = {
        "vs_currency": "usd",
        "days": "1",
        "interval": "hourly"
    }
    response = requests.get(url, params=params)
    data = response.json()
    
    # Convert to DataFrame
    prices = pd.DataFrame(data['prices'], columns=['timestamp', 'price'])
    prices['timestamp'] = pd.to_datetime(prices['timestamp'], unit='ms')
    prices.set_index('timestamp', inplace=True)
    
    return prices

# Prepare the dataset for prediction
def prepare_data(prices):
    prices['price_change'] = prices['price'].diff().shift(-1)  # Calculate next hour's price change
    prices.dropna(inplace=True)
    X = prices['price'].values.reshape(-1, 1)
    y = (prices['price_change'] > 0).astype(int)  # Label: 1 if price increases, 0 otherwise
    
    return train_test_split(X, y, test_size=0.2, random_state=42)

# Train a logistic regression model
def train_model(X_train, y_train):
    model = LogisticRegression()
    model.fit(X_train, y_train)
    return model

# Predict future price change
def predict_price_change(model, last_hour_price):
    prediction = model.predict(np.array([[last_hour_price]]))
    return "increase" if prediction[0] == 1 else "decrease"

if __name__ == "__main__":
    # Fetch data
    prices = fetch_bitcoin_data()
    
    # Prepare dataset
    X_train, X_test, y_train, y_test = prepare_data(prices)
    
    # Train model
    model = train_model(X_train, y_train)
    
    # Evaluate model (optional)
    y_pred = model.predict(X_test)
    accuracy = accuracy_score(y_test, y_pred)
    print(f"Model Accuracy: {accuracy:.2f}")
    
    # Predict next hour's price change
    last_hour_price = prices['price'].iloc[-1]
    prediction = predict_price_change(model, last_hour_price)
    print(f"Predicted price change in the next hour: {prediction}")
```

### Running the Application
To run the application, save the code to `bitcoinPrediction2.py` and execute it:
```sh
python bitcoinPrediction2.py
```

### Notes
1. **Data Source**: The CoinGecko API provides historical data for Bitcoin prices in USD.
2. **Model Complexity**: This example uses a simple logistic regression model. For better accuracy, consider using more advanced models like Random Forests, LSTM networks, or even transformers.
3. **Feature Engineering**: More features (e.g., trading volume, market capitalization) can be added to improve prediction accuracy.
4. **Real-time Data**: For real-time predictions, you would need to fetch the latest data and update your model accordingly.

This example should give you a starting point for predicting Bitcoin price changes using Python.
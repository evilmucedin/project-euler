import requests
import pandas as pd
import numpy as np
from datetime import datetime
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
import time
import warnings

# Suppress warnings
warnings.filterwarnings('ignore')

class BitcoinPredictor:
    def __init__(self):
        self.api_url = "https://api.coingecko.com/api/v3"
        self.model = None
        self.scaler = None
        self.is_trained = False
        
    def fetch_historical_data(self, days=30):
        """Fetch historical Bitcoin price data from CoinGecko"""
        try:
            # Calculate end_timestamp (current time)
            end_timestamp = int(time.time())
            start_timestamp = end_timestamp - (days * 24 * 60 * 60)
            
            # Fetch hourly OHLC data
            url = f"{self.api_url}/coins/bitcoin/ohlc?vs_currency=usd&days={days}"
            
            print(f"📡 Fetching Bitcoin data from CoinGecko...")
            print(f"   Time range: {days} days")
            
            print(f"Days: {days}")
            response = requests.get(url, timeout=300)
            print(f"Days2: {days, response.status_code}")
            
            if response.status_code == 429:
                print("⚠️ Rate limit reached, waiting before retry...")
                time.sleep(5)
                response = requests.get(url, timeout=30)
            
            if response.status_code != 200:
                print(f"❌ Error fetching data: {response.status_code}")
                print(f"   Error message: {response.text}")
                raise Exception(f"API Error: {response.status_code}")
            
            data = response.json()
            print(f"✅ Successfully fetched {len(data)} data points")
            print(f"Data3: {data}")
            
            return data
            
        except Exception as e:
            print(f"❌ Error in fetch_historical_data: {str(e)}")
            raise
            
    def process_data(self, data):
        """Process raw data into features"""
        print(f"Data5: {len(data)}")
        df = pd.DataFrame(data, columns=['timestamp', 'open', 'high', 'low', 'close'])
        print(f"Data6: {len(data)}")
        
        # Convert timestamp to datetime
        print(f"Data6666: {len(data)}")
        # df['timestamp'] = pd.to_datetime(df['timestamp'], unit='s')
        print(f"Data666: {len(data)}")
        df.set_index('timestamp', inplace=True)
        
        # Calculate price features
        df['price_change'] = df['close'].pct_change()
        df['volume_change'] = df['close'] - df['close'].shift(1)
        print(f"Data6: {len(data)}")
        
        # Calculate technical indicators
        print(f"Data77: {len(data)}")
        df['SMA_5'] = df['close'].rolling(window=5).mean()
        print(f"Data7: {len(data)}")
        df['SMA_10'] = df['close'].rolling(window=10).mean()
        print(f"Data8: {len(data)}")
        df['RSI'] = self.calculate_rsi(df['close'], period=14)
        print(f"Data9: {len(data)}")
        df['volatility'] = df['close'].rolling(window=10).std()
        print(f"Data7: {len(data)}")
        
        # Calculate price range
        df['price_range'] = (df['high'] - df['low']) / df['close']
        
        # Create target variable (1 if next hour price is higher, 0 if lower)
        df['target'] = (df['close'].shift(-1) > df['close']).astype(int)
        
        # Drop NaN values
        df.dropna(inplace=True)
        
        return df
    
    def calculate_rsi(self, series, period=14):
        """Calculate RSI indicator"""
        delta = series.diff()
        gain = (delta.where(delta > 0, 0)).rolling(window=period).mean()
        loss = (-delta.where(delta < 0, 0)).rolling(window=period).mean()
        rs = gain / loss
        rsi = 100 - (100 / (1 + rs))
        return rsi
    
    def create_features(self, df):
        """Create feature set from processed data"""
        features = df[['price_change', 'volume_change', 'SMA_5', 'SMA_10', 
                       'RSI', 'volatility', 'price_range', 'close']].copy()
        
        return features, df['target']
    
    def train_model(self, data):
        """Train the prediction model"""
        print("🎯 Training prediction model...")
        print(f"Data2: ")
        print(f"Data2: {len(data)}")
        
        # Process data
        df = self.process_data(data)
        print(f"Data22: {len(data)}")
        
        # Create features
        X, y = self.create_features(df)
        
        # Split data (80% train, 20% test)
        X_train, X_test, y_train, y_test = train_test_split(
            X, y, test_size=0.2, shuffle=False, random_state=42
        )
        
        # Scale features
        self.scaler = StandardScaler()
        X_train_scaled = self.scaler.fit_transform(X_train)
        X_test_scaled = self.scaler.transform(X_test)
        
        # Train Random Forest model
        self.model = RandomForestClassifier(
            n_estimators=100,
            max_depth=10,
            random_state=42,
            class_weight='balanced'
        )
        self.model.fit(X_train_scaled, y_train)
        
        # Evaluate model
        accuracy = self.model.score(X_test_scaled, y_test)
        print(f"   Model accuracy: {accuracy:.2%}")
        
        # Store for later use
        self.training_features = X_train
        self.training_target = y_train
        self.is_trained = True
        
        print("✅ Model training complete!")
        return accuracy
    
    def predict_next_hour(self):
        """Predict if Bitcoin price will go up or down in next hour"""
        if not self.is_trained:
            print("❌ Model not trained yet!")
            return None
            
        try:
            # Fetch latest data
            data = self.fetch_historical_data(days=1)
            print(f"Data77: {len(data)}")
            df = self.process_data(data)
            print(f"DF: {df}")
            
            # Get last row
            last_row = df.iloc[-1]
            
            # Create feature vector
            features = pd.DataFrame([{
                'price_change': last_row['price_change'],
                'volume_change': last_row['volume_change'],
                'SMA_5': last_row['SMA_5'],
                'SMA_10': last_row['SMA_10'],
                'RSI': last_row['RSI'],
                'volatility': last_row['volatility'],
                'price_range': last_row['price_range'],
                'close': last_row['close']
            }])
            
            # Scale features
            features_scaled = self.scaler.transform(features)
            
            # Make prediction
            prediction = self.model.predict(features_scaled)[0]
            probabilities = self.model.predict_proba(features_scaled)[0]
            
            # Get current price
            current_price = last_row['close']
            
            return {
                'prediction': 'UP 📈' if prediction == 1 else 'DOWN 📉',
                'current_price': current_price,
                'confidence': max(probabilities) * 100,
                'probability_up': probabilities[1] * 100,
                'probability_down': probabilities[0] * 100,
                'rsi': last_row['RSI'],
                'volatility': last_row['volatility']
            }
            
        except Exception as e:
            print(f"❌ Error in prediction: {str(e)}")
            return None
    
    def print_prediction(self, prediction):
        """Print prediction results"""
        if prediction is None:
            print("❌ Prediction failed!")
            return
            
        print("\n" + "="*60)
        print("🤖 BITCOIN PRICE PREDICTION - NEXT HOUR")
        print("="*60)
        print(f"📊 Current Price: ${prediction['current_price']:,.2f}")
        print(f"🔮 Prediction: {prediction['prediction']}")
        print(f"📈 Probability Up: {prediction['probability_up']:.2f}%")
        print(f"📉 Probability Down: {prediction['probability_down']:.2f}%")
        print(f"🎯 Confidence Level: {prediction['confidence']:.2f}%")
        print(f"📊 RSI: {prediction['rsi']:.2f}")
        print(f"📉 Volatility: {prediction['volatility']:.6f}")
        print("="*60)
        
        # Trading signal
        if prediction['probability_up'] > 60:
            print("💡 Suggestion: Consider BUY signal")
        elif prediction['probability_down'] > 60:
            print("💡 Suggestion: Consider SELL/SHORT signal")
        else:
            print("💡 Suggestion: Hold/Wait for clearer signal")
        print("="*60)

def main():
    """Main execution function"""
    print("🚀 Bitcoin Price Predictor - Starting...")
    print("="*60)
    
    try:
        # Initialize predictor
        predictor = BitcoinPredictor()
        
        # Train the model with historical data
        print("\n📊 Step 1: Downloading historical data for training...")
        historical_data = predictor.fetch_historical_data(days=7)
        
        print("\n📊 Step 2: Training prediction model...")
        accuracy = predictor.train_model(historical_data)
        
        # Make prediction
        print("\n📊 Step 3: Making prediction for next hour...")
        prediction = predictor.predict_next_hour()
        
        # Display results
        predictor.print_prediction(prediction)
        
        # Important disclaimer
        print("\n⚠️  DISCLAIMER:")
        print("   - This is for educational purposes only")
        print("   - Do not use for real trading decisions")
        print("   - Cryptocurrency trading involves high risk")
        print("   - Past performance ≠ future results")
        print("="*60)
        
    except KeyboardInterrupt:
        print("\n\n⚠️  Program interrupted by user")
    except Exception as e:
        print(f"\n❌ Fatal error: {str(e)}")
        print("   Please check your internet connection and try again")

if __name__ == "__main__":
    main()

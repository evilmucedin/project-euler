#!/usr/bin/env python
import pandas as pd
import pyarrow.parquet as pq

# Read Parquet files
print("Reading Parquet files...")
employees_df = pd.read_parquet('employees.parquet')
sales_df = pd.read_parquet('sales.parquet')
products_df = pd.read_parquet('products.parquet')

print("=" * 60)
print("QUERY 1: Top earners")
print("=" * 60)
top_earners = employees_df.nlargest(3, 'salary')[['name', 'department', 'salary']]
print(top_earners)

print("\n" + "=" * 60)
print("QUERY 2: Sales by employee")
print("=" * 60)
sales_by_employee = sales_df.merge(employees_df[['employee_id', 'name']], on='employee_id')
employee_totals = sales_by_employee.groupby('name')['amount'].sum().sort_values(ascending=False)
print(employee_totals)

print("\n" + "=" * 60)
print("QUERY 3: High-value products (price > $500)")
print("=" * 60)
high_value_products = products_df[products_df['price'] > 500][['product_name', 'price', 'stock']]
print(high_value_products)

print("\n" + "=" * 60)
print("QUERY 4: Average salary by department")
print("=" * 60)
avg_salary_by_dept = employees_df.groupby('department')['salary'].mean().round(2)
print(avg_salary_by_dept)

print("\n" + "=" * 60)
print("QUERY 5: Products with low stock (< 100)")
print("=" * 60)
low_stock = products_df[products_df['stock'] < 100][['product_name', 'stock', 'price']]
print(low_stock)

print("\n✓ All queries executed successfully!")

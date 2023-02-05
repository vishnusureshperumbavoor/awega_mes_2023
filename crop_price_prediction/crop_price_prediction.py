import pandas as pd
import datetime
import xgboost as xgb
import streamlit as st

def main():
    html_temp="""
        <div style="background-color:lightblue;padding:16px">
            <h2 style="color:black;text-align:center;">Crop Price Prediction using Deep Learning</h2>
        </div>
    """
    st.markdown(html_temp,unsafe_allow_html=True)
    st.write('')
    st.write('')
    st.markdown("##### Do you want us to predict the short term and long term price of the crop? \n ")
    st.markdown("##### Let's try evaluating the price")

    st.selectbox("Which is the district?",('Kerala','TamilNadu','Karnataka'))
    
if __name__ == "__main__":
    main()
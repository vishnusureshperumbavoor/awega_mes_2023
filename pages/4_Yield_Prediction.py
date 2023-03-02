import pandas as pd
import streamlit as st
from joblib import load
import datetime

def main():
    rfr_model = load('crop_price_prediction.json')
    html_temp="""
        <div style="background-color:lightblue;padding:16px">
            <h2 style="color:black;text-align:center;">Crop Yield Prediction using eXtreme Gradient Boosting</h2>
        </div>
    """
    st.markdown(html_temp,unsafe_allow_html=True)
    st.write('')
    st.write('')
    #st.markdown("##### Do you want us to recommend crops? \n ")

    p1 = st.selectbox("Select District",('Ernakulam',''))
    if p1=="Ernakulam":
        p1=0

    
    

    p3 = st.selectbox("Select Crop",('Banana',''))
    if p3=="Banana":
        p3=0

    p4 = st.selectbox("Select Variety",('Palayamthodan','Poovan','Besrai','others'))
    if p4=="Palayamthodan":
        p4=0
    elif p4=="Poovan":
        p4=1
    elif p4=="Besrai":
        p4=2
    elif p4=="Others":
        p4=3

    p5 = st.selectbox("Select Variety",('Large','Medium','Small'))
    if p5=="Large":
        p5=0 
    elif p5=="Medium":
        p5=1
    elif p5=="Small":
        p5=2

    present_date = datetime.date.today()
    min_date = datetime.date(2023,1,1)
    max_date = datetime.date(2023,12,31)
    p6 = st.date_input("Select month & day",present_date,min_date,max_date)

    month = p6.month
    if month==1 or month==2:
        p7=0
    elif month==3 or month==4:
        p7=1
    elif month==5 or month==6:
        p7=2 
    elif month==7 or month==8:
        p7=3
    elif month==9 or month==10:
        p7=4 
    elif month==11 or month==12:
        p7=5

    user_input = pd.DataFrame({
        'District Name':p1,
        'Market Name':p2,
        'Commodity':p3,
        'Variety':p4,
        'Grade':p5,
        'month_column':p6.month,
        'season_names':p7,
        'day':p6.weekday(),
    },index=[0])

    try:
        if st.button('Predict'):
            print(p6)
            print(p6.day)
            pred = rfr_model.predict(user_input)
            #st.balloons()
            st.success("Modal Price = {:.2f} Rs./Quintal".format(pred[0]))
    except:
        st.warning("You can't cultivate crops in this land")


    
if __name__ == "__main__":
    main()
import pandas as pd
import streamlit as st
from joblib import load

def main():
    dtc_model = load('crop_recommendation.json')
    html_temp="""
        <div style="background-color:lightblue;padding:16px">
            <h2 style="color:black;text-align:center;">Crop Recommendation using Machine Learning</h2>
        </div>
    """
    st.markdown(html_temp,unsafe_allow_html=True)
    st.write('')
    st.write('')
    st.markdown("##### Do you want us to recommend crops? \n ")

    p1 = st.number_input("What is N (Nitrogen) value?",0,140,step=1)
    p2 = st.number_input("What is P (Phosphorus) value?",5,145,step=1)
    p3 = st.number_input("What is K (Potassium) value?",5,205,step=1)
    p4 = st.number_input("How much temperature?",8.82,44.67,step=1.00)
    p5 = st.number_input("How much humidity?",14.25,99.98,step=1.00)
    p6 = st.number_input("What is the pH value?",3.50,9.93,step=1.00)
    p7 = st.number_input("How much rainfall?",20.21,213.90,step=1.00)

    data_new = pd.DataFrame({
        'N':p1,
        'P':p2,
        'K':p3,
        'temperature':p3,
        'humidity':p4,
        'ph':p5,
        'rainfall':p6,
    },index=[0])

    if st.button('Predict'):
        pred = dtc_model.predict(data_new)
        #st.balloons()
        st.success("You can cultivate {}".format(pred[0]))


    
if __name__ == "__main__":
    main()
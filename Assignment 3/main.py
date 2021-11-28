# Alex Young and Steven Bui
# CS 331 Intro to AI - Assignment 3: Sentiment Analysis
# 5/24/2021
# run this file with python3 main.py

import re
import math

def create_vocab(input):
    vocab = {}
    with open(input, "r") as f:
        for lines in f:
            line = re.sub(r"[']+", "", lines.lower())
            line = re.sub(r'[^A-Za-z]+', ' ', line)
            temp = line.split()
            for word in temp:
                if word not in vocab:
                    vocab[word] = 0
    return vocab

def pre_process(input, output, vocab):
    array = []
    with open(input, "r") as f:
        for lines in f:
            vector = vocab.copy()
            vector['classlabel'] = int(lines[len(lines) - 3])
            line = re.sub(r"[']+", "", lines.lower())
            line = re.sub(r'[^A-Za-z]+', ' ', line)
            temp = line.split()
            for word in temp:
                if word in vocab:
                    vector[word] = 1
            array.append(vector)

    f = open(output, "w")
    for words in sorted(array[0].keys()):
        if words != 'classlabel':
            f.write("%s," %words)
    f.write("classlabel\n")
    for vectors in array:
        for words in sorted(vectors.keys()):
            if words != 'classlabel':
                f.write("%i," %vectors.get(words))
        f.write("%i\n" %vectors.get('classlabel'))
    f.close()
    return array

def classify(train, test, vocab):
    # training phase
    count_neg_word = vocab.copy()
    count_pos_word = vocab.copy()
    total_count_word = vocab.copy()
    num_neg = 0
    num_pos = 0
    # probablilty of a word given the class label
    p_word_neg = vocab.copy()
    p_word_pos = vocab.copy()

    for vectors in train:
        if vectors.get('classlabel') == 0:
            num_neg += 1
            for words in vectors.keys():
                if vectors.get(words) == 1:
                    count_neg_word[words] += 1
                    total_count_word[words] += 1
        
        if vectors.get('classlabel') == 1:
            num_pos += 1
            for words in vectors.keys():
                if vectors.get(words) == 1 and words != 'classlabel':
                    count_pos_word[words] += 1
                    total_count_word[words] += 1

    p_neg = num_neg / (num_neg + num_pos)
    p_pos = num_pos / (num_neg + num_pos)

    for words in p_word_neg.keys():
        p_word_neg[words] = (count_neg_word[words] + 1) / (num_neg + 2)
        
    for words in p_word_pos.keys():
        p_word_pos[words] = (count_pos_word[words] + 1) / (num_pos + 2)

    # now we are in testing phase=
    correct_predictions = 0
    total_predictions = 0

    for reviews in test:
        neg_sum = 0
        pos_sum = 0
        for words in reviews.keys():
            if words != 'classlabel' and reviews[words] == 1:
                neg_sum += math.log(p_word_neg[words])
                pos_sum += math.log(p_word_pos[words])
        prediction_neg = math.log(p_neg) + neg_sum
        prediction_pos = math.log(p_pos) + pos_sum

        total_predictions += 1
        if prediction_neg > prediction_pos and reviews.get('classlabel') == 0:
            correct_predictions += 1
        elif prediction_pos >= prediction_neg and reviews.get('classlabel') == 1:
            correct_predictions += 1
    return correct_predictions / total_predictions

# run this file with python3 main.py
trained_vocab = create_vocab('trainingSet.txt')
prep_train = pre_process('trainingSet.txt', 'preprocessed_train.txt', trained_vocab)
prep_test = pre_process('testSet.txt', 'preprocessed_test.txt', trained_vocab)

f = open("results.txt", "w")
accuracy = classify(prep_train, prep_train, trained_vocab)
f.write("Trained Set Accuracy: %f percent\nTraining file used: %s, Testing file used: %s" %(accuracy * 100, 'trainingSet.txt', 'trainingSet.txt'))
accuracy = classify(prep_train, prep_test, trained_vocab)
f.write("\n\nTest Set Accuracy: %f percent\nTraining file used: %s, Testing file used: %s" %(accuracy * 100, 'trainingSet.txt', 'testSet.txt'))
f.close()